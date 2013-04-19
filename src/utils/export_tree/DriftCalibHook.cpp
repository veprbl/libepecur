#include <cstring>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>
#include <epecur/StdDrift.hpp>

#include "DriftCalibHook.hpp"

DriftCalibHook::DriftCalibHook( Geometry &g )
    : StdDrift(g)
{
	// Nothing
}

DriftCalibHook::~DriftCalibHook()
{
	// Nothing
}

void	DriftCalibHook::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	StdDrift::handle_drift_data(wire_id_s, time_s, dev_id);
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

	StdDrift::handle_event_end();
}
