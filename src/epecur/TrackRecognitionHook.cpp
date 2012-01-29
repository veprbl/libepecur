#include "TrackRecognitionHook.hpp"

TrackRecognitionHook::TrackRecognitionHook( Geometry &g, double max_chisq ) : last_event_finished(false), max_chisq(max_chisq), geom(g)
{
	// nothing
}

void	TrackRecognitionHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	if (last_event_finished)
	{
		last_event.clear();

		for(auto gr_tup : last_tracks)
		{
			for(auto axis_tup : gr_tup.second)
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

void	TrackRecognitionHook::handle_event_end()
{
	vector< vector<wire_pos_t>* >	block;

	for(auto gr_tup : geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	chambers = axis_tup.second;

			block.clear();

			for(chamber_id_t chamber_id : chambers)
			{
				block.push_back(&last_event[chamber_id]);
			}

			vector<double>	&normal_pos = geom.group_normal_pos[1][DEV_AXIS_X];

			last_tracks[group_id][axis] = prop_recognize_all_tracks(block, normal_pos, max_chisq);
		}
	}

	last_event_finished = true;
}
