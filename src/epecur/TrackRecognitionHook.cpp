#include <cstdlib>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "TrackRecognitionHook.hpp"

TrackRecognitionHook::TrackRecognitionHook(
	Geometry &g,
	StdDrift::calibration_curve_t *c
	)
	: StdDrift(g),
	  last_event_finished(false),
	  geom(g),
	  calibration_curve(c)
{
	// nothing
}

void	TrackRecognitionHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	if (last_event_finished)
	{
		last_event.clear();

		BOOST_FOREACH(auto gr_tup, last_tracks)
		{
			BOOST_FOREACH(auto axis_tup, gr_tup.second)
			{
				axis_tup.second.clear();
			}

		}

		last_event_finished = false;
	}

	auto &chamber = last_event[chamber_id];
	wire_id_t	prev_pos;
	int	sequence_len = 1, max_sequence_len = 3;

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

void	TrackRecognitionHook::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	StdDrift::handle_drift_data(wire_id_s, time_s, dev_id);

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	// if calibration curve is not provided, just use wire_id's
	if (calibration_curve == NULL)
	{
		last_event[chamber_id] = last_event_drift_wire_pos[chamber_id];
		return;
	}

	auto	&wire_pos = last_event_drift_wire_pos[chamber_id];
	auto	&time = last_event_drift_time[chamber_id];
	auto	&calib = (*calibration_curve)[chamber_id];
	auto	wit = wire_pos.begin();
	auto	tit = time.begin();

	while(wit != wire_pos.end())
	{
		const double	DRIFT_DISTANCE = 17.0;
		last_event[chamber_id].push_back(DRIFT_DISTANCE*(*wit + 0.5 * calib[*tit]));
		last_event[chamber_id].push_back(DRIFT_DISTANCE*(*wit - 0.5 * calib[*tit]));
		wit++;
		tit++;
	}
}

void	TrackRecognitionHook::handle_event_end()
{
	vector< vector<wire_pos_t>* >	block;

	BOOST_FOREACH(auto &gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];
		double	max_chisq = geom.group_max_chisq[group_id];

		BOOST_FOREACH(auto &axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			const vector<chamber_id_t>	&chambers = axis_tup.second;

			block.clear();

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block.push_back(&last_event[chamber_id]);
			}

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			if (device_type == DEV_TYPE_PROP)
			{
				last_tracks[group_id][axis] = recognize_all_tracks<track_type_t::prop>(block, normal_pos, max_chisq);
			}
			else if (device_type == DEV_TYPE_DRIFT)
			{
				last_tracks[group_id][axis] = recognize_all_tracks<track_type_t::drift>(block, normal_pos, max_chisq);
			}
			else
			{
				throw "Invalid dev type";
			}
		}
	}

	StdDrift::handle_event_end();
	last_event_finished = true;
}
