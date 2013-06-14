#include <cstring>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <epecur/types.hpp>
#include <epecur/track.hpp>
#include <epecur/StdDrift.hpp>

#include "DriftCalibHook.hpp"

DriftCalibHook::DriftCalibHook( Geometry &g )
    : TrackRecognitionHook(g, NULL) // NULL will make TRH recognize rough drift tracks
    , drift_calib("drift_calib", "drift chambers calibration curves")
    , calib_curve(
        "calib_curve", "",
        MAX_TIME_COUNTS, 0, MAX_TIME_COUNTS
        )
{
	drift_calib.Branch("calib_curve", "TH1F", &calib_curve);
}

unsigned int	DriftCalibHook::generate_calibration_curve( chamber_id_t chamber_id )
{
	unsigned int	result = 0;
	auto	&calib = calibration_curve[chamber_id];

	calib.resize(MAX_ANGLE_COUNTS);

	for(small_angle_t psi = 0; psi < MAX_ANGLE_COUNTS; psi++)
	{
		uint16_t	time = 0;
		unsigned int	overal_integral = 0, integral = 0;

		calib[psi].resize(MAX_TIME_COUNTS);

		BOOST_FOREACH(auto counts, time_distributions[chamber_id][psi])
		{
			overal_integral += counts;
		}

		BOOST_FOREACH(auto counts, time_distributions[chamber_id][psi])
		{
			integral += counts;
			auto	value = integral / (float)overal_integral;
			calib[psi][time] = value;
			calib_curve.Fill(time, value);
			time++;
		}

		result += overal_integral;
	}

	return result;
}

void	DriftCalibHook::generate_calibration_curves()
{
	string	title;

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type != DEV_TYPE_DRIFT)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	&chambers =
				geom.group_chambers[group_id][axis];
			int	chamber_num = 1;

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				auto	num_events = generate_calibration_curve(chamber_id);

				title = "d" +
					boost::lexical_cast<string>(int(group_id)) +
					((axis == DEV_AXIS_X) ? "X" : "Y") +
					boost::lexical_cast<string>(int(chamber_num)) +
					" num_events = " +
					boost::lexical_cast<string>(num_events);

				calib_curve.SetTitle(title.c_str());

				drift_calib.Fill();
				calib_curve.Reset();

				chamber_num++;
			}
		}
	}
}

void	DriftCalibHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		if (geom.group_device_type[group_id] != DEV_TYPE_DRIFT)
		{
			continue;
		}

		BOOST_FOREACH(auto &axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	&chambers =
			    geom.group_chambers[group_id][axis];
			vector<track_info_t>	&tracks = last_tracks[group_id][axis];

			BOOST_FOREACH(track_info_t &track, tracks)
			{
				auto	wire_index_it = track.wire_pos_ptr.begin();
				small_angle_t	psi = c1_to_angle(track.c1);

				BOOST_FOREACH(auto chamber_index, track.used_chambers)
				{
					chamber_id_t	chamber_id = chambers[chamber_index];
					uint16_t	t =
					    last_event_drift_time[chamber_id][*(wire_index_it++)];

					auto	&calib = time_distributions[chamber_id];

					if (calib.empty())
					{
						calib.resize(MAX_ANGLE_COUNTS);
					}

					if (calib[psi].empty())
					{
						calib[psi].resize(MAX_TIME_COUNTS);
					}

					calib[psi][t]++;
				}
			}
		}
	}

	StdDrift::handle_event_end();
}
