#include <cstring>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <epecur/types.hpp>
#include <epecur/track.hpp>
#include <epecur/StdHits.hpp>

#include "DriftCalibHook.hpp"

DriftCalibHook::DriftCalibHook(Geometry &g, int _drift_calib_cut)
    : StdHits(g, NULL)
    , drift_calib_cut(_drift_calib_cut)
{
	// Nothing
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
		time++;
	}

	return overal_integral;
}

void	DriftCalibHook::generate_calibration_curves()
{
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

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				generate_calibration_curve(chamber_id);
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
				vector<wire_pos_t> &wire_pos = last_event_drift_wire_pos[chamber_id];
				auto it = wire_pos.begin();

				BOOST_FOREACH(uint16_t time, last_event_drift_time[chamber_id])
				{
					auto	&calib = time_distributions[chamber_id];

					// Here we are removing hits from the adjacent cells
					bool fail = false;
					for(auto it2 = wire_pos.begin(); it2 != it; it2++)
					{
						if (fabs(*it - *it2) <= 2.001)
						{
							fail = true;
							break;
						}
					}
					if (fail)
						break;
					it++;

					if (calib.empty())
					{
						calib.resize(MAX_TIME_COUNTS);
					}

					if ((drift_calib_cut < 0) || (time < drift_calib_cut))
					{
						calib[time]++;
					}
				}
			}
		}
	}
}
