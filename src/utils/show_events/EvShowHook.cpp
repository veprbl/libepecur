#include <cstring>
#include <algorithm>
#include <iostream>

#include <epecur/types.hpp>

#include "EvShowHook.hpp"

EvShowHook::EvShowHook( Geometry &g ) : geom(g), event_id(0)
{
	// nothing
}

void	EvShowHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	auto	&event = events[event_id];
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	if (event.count(chamber_id) == 0)
	{
		event[chamber_id] = vector<wire_pos_t>();
	}

	auto &chamber = event[chamber_id];

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		chamber.push_back(wire_pos);
	}
}

void	EvShowHook::handle_event_end()
{
	auto	&event = events[event_id];

	vector< vector<wire_pos_t>* >	block;

	for(chamber_id_t chamber_id : geom.group_chambers[1][DEV_AXIS_X] )
	{
		vector<wire_pos_t>	&chamber_wires = event[chamber_id];

		block.push_back(&chamber_wires);
	}

	vector<double>	&normal_pos = geom.group_normal_pos[1][DEV_AXIS_X];

	tracks[event_id] = prop_recognize_all_tracks(block, normal_pos);

	vector<double>::iterator	max_pos = max_element(normal_pos.begin(), normal_pos.end());
	vector<double>::iterator	min_pos = min_element(normal_pos.begin(), normal_pos.end());

	for(track_info_t &track : tracks[event_id])
	{
		track.c1 *= *max_pos - *min_pos;
	}

	event_id++;

	if (event_id >= LOAD_EVENTS_COUNT)
	{
		throw 7;
	}
}
