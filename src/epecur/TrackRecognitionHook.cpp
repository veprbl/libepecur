#include <boost/foreach.hpp>

#include "TrackRecognitionHook.hpp"

TrackRecognitionHook::TrackRecognitionHook( Geometry &g, double max_chisq )
	: last_event_finished(false),
	  drift_cleanup(false),
	  max_chisq(max_chisq),
	  geom(g)
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

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		chamber.push_back(wire_pos);
	}
}

void	TrackRecognitionHook::handle_drift_data(
	vector< pair<wire_id_t, uint16_t> > &wires, device_id_t dev_id
	)
{
	wire_id_t	wire_id;
	uint16_t	time;

	if (drift_cleanup)
	{
		last_event_drift.clear();

		drift_cleanup = false;
	}

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);
	auto	&chamber = last_event_drift[chamber_id];

	BOOST_FOREACH(auto wire, wires)
	{
		std::tie(wire_id, time) = wire;
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, wire_id);

		chamber.emplace_back(wire_pos, time);
	}
}

void	TrackRecognitionHook::handle_event_end()
{
	vector< vector<wire_pos_t>* >	block;

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	chambers = axis_tup.second;

			block.clear();

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block.push_back(&last_event[chamber_id]);
			}

			vector<double>	&normal_pos = geom.group_normal_pos[group_id][axis];

			last_tracks[group_id][axis] = prop_recognize_all_tracks(block, normal_pos, max_chisq);
		}
	}

	last_event_finished = true;
	drift_cleanup = true;
}
