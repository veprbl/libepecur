#include <cmath>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "StdDrift.hpp"

void	StdDrift::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	if (drift_cleanup)
	{
		last_event_drift_wire_pos.clear();
		last_event_drift_time.clear();

		drift_cleanup = false;
	}

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);
	auto	&event_wire_pos = last_event_drift_wire_pos[chamber_id];
	auto	&event_time = last_event_drift_time[chamber_id];

	BOOST_ASSERT(wire_id_s.size() == time_s.size());

	auto	time_it = time_s.begin();
	wire_pos_t	prev_wire_pos = 0;
	uint16_t	prev_time = 0;

	if (!event_wire_pos.empty())
	{
		prev_wire_pos = event_wire_pos.back();
		prev_time = event_time.back();
	}

	BOOST_FOREACH(wire_id_t wire_id, wire_id_s)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, wire_id);
		uint16_t	time = (*time_it);
		bool		append = true;

		if ((!event_wire_pos.empty())
		    && (abs(prev_wire_pos - wire_pos) == 2))
		{
			int16_t	timedelta = time - prev_time;

			if ((timedelta >= -4) && (timedelta < 0))
			{
				event_wire_pos.back() = wire_pos;
				event_time.back() = time;
				prev_wire_pos = wire_pos;
				prev_time = time;

				append = false;
			}

			if ((timedelta > 0) && (timedelta <= 4))
			{
				append = false;
			}
		}

		if (append)
		{
			event_wire_pos.push_back(wire_pos);
			event_time.push_back(time);
			prev_wire_pos = wire_pos;
			prev_time = time;
		}

		time_it++;
	}

	BOOST_ASSERT(event_wire_pos.size() == event_time.size());
}

void	StdDrift::handle_event_end()
{
	drift_cleanup = true;
}
