#include <cstring>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <epecur/types.hpp>
#include <epecur/track.hpp>
#include <epecur/StdHits.hpp>

#include "DriftCalibHook.hpp"

DriftCalibHook::DriftCalibHook( Geometry &g )
    : StdHits(g, NULL)
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
	uint16_t	time = 0;
	unsigned int	overal_integral = 0, integral = 0;
	auto	&calib = calibration_curve[chamber_id];

	calib.resize(MAX_TIME_COUNTS);

	BOOST_FOREACH(auto counts, time_distributions[chamber_id])
	{
		overal_integral += counts;
	}

	BOOST_FOREACH(auto counts, time_distributions[chamber_id])
	{
		integral += counts;
		auto	value = integral / (float)overal_integral;
		calib[time] = value;
		calib_curve.Fill(time, value);
		time++;
	}

	return overal_integral;
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
				calib_curve.SetLabelSize(0.04, "X");
				calib_curve.SetLabelSize(0.04, "Y");
				calib_curve.SetTitleSize(0.05, "X");
				calib_curve.SetTitleSize(0.05, "Y");
				calib_curve.SetTitleOffset(0.9, "X");
				calib_curve.SetTitleOffset(0.9, "Y");
				calib_curve.GetXaxis()->SetTitle("T, [Counts]");
				calib_curve.GetYaxis()->SetTitle("x/(8.5 mm)");

				drift_calib.Fill();
				calib_curve.Reset();

				chamber_num++;
			}
		}
	}
}

void	DriftCalibHook::handle_event_end()
{
	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		if (geom.group_device_type[group_id] != DEV_TYPE_DRIFT)
		{
			continue;
		}

		BOOST_FOREACH(auto &axis_tup, gr_tup.second)
		{
			const vector<chamber_id_t>	&chambers = axis_tup.second;

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				BOOST_FOREACH(uint16_t time, last_event_drift_time[chamber_id])
				{
					auto	&calib = time_distributions[chamber_id];

					if (calib.empty())
					{
						calib.resize(MAX_TIME_COUNTS);
					}

					calib[time]++;
				}
			}
		}
	}

	StdHits::handle_event_end();
}
