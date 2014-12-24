#include <cmath>
#include <bitset>
#include <iostream>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "StdHits.hpp"

void	StdHits::handle_prop_data(const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id)
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);
	auto &chamber = last_event[chamber_id];
	wire_id_t	prev_pos;
	unsigned int	sequence_len = 1, max_sequence_len = 3;

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		if ((!chamber.empty()) && (prev_pos == (*pos) - 1))
		{
			sequence_len++;
		}
		else
		{
			if (sequence_len > max_sequence_len)
			{
				chamber.resize(chamber.size() - sequence_len);
			}
			sequence_len = 1;
		}

		chamber.push_back(wire_pos);
		BOOST_ASSERT(chamber.size() >= sequence_len);

		prev_pos = *pos;
	}

	if (sequence_len > max_sequence_len)
	{
		chamber.resize(chamber.size() - sequence_len);
	}
}

void	StdHits::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	const double	DRIFT_DISTANCE = 17.0/2;

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);
	auto	&event_wire_pos = last_event_drift_wire_pos[chamber_id];
	auto	&event_time = last_event_drift_time[chamber_id];
	auto	&event = last_event[chamber_id];
	vector<double>		*calib = NULL;
	if (calibration_curve != NULL)
	{
		calib = &((*calibration_curve)[chamber_id]);
	}

	BOOST_ASSERT(wire_id_s.size() == time_s.size());

	if ((calibration_curve != NULL) && calib->empty())
	{
		static bitset<256>	warnings_generated;
		bool	generated = warnings_generated[chamber_id];
		if (!generated)
		{
			cerr << "No calibration curve for chamber " << int(chamber_id)
			     << ". Data is ignored." << endl;
		}
		warnings_generated.set(chamber_id);
		return;
	}

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
				if (calib == NULL)
				{
					event.back() = wire_pos * DRIFT_DISTANCE;
				}
				else
				{
					*(event.end()-2) = (wire_pos + (*calib)[time]) * DRIFT_DISTANCE;
					*(event.end()-1) = (wire_pos - (*calib)[time]) * DRIFT_DISTANCE;
				}
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
			if (calib == NULL)
			{
				event.push_back(wire_pos * DRIFT_DISTANCE);
			}
			else
			{
				event.push_back((wire_pos + (*calib)[time]) * DRIFT_DISTANCE);
				event.push_back((wire_pos - (*calib)[time]) * DRIFT_DISTANCE);
			}
			prev_wire_pos = wire_pos;
			prev_time = time;
		}

		time_it++;
	}

	BOOST_ASSERT(event_wire_pos.size() == event_time.size());
}

void	StdHits::handle_event_start()
{
	last_event_drift_wire_pos.clear();
	last_event_drift_time.clear();

	BOOST_FOREACH(auto &tup, last_event)
	{
		tup.second.clear();
	}
}
