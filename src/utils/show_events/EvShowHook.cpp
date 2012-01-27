#include <cstring>
#include <iostream>

#include <epecur/types.hpp>

#include "EvShowHook.hpp"

EvShowHook::EvShowHook( Geometry &g ) : geom(g)
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

	for(auto &chamber : event)
	{
		chamber_id_t	chamber_id = chamber.first;
		vector<wire_pos_t>	&chamber_wires = chamber.second;

		if ((chamber_id % 2 == 0) && (chamber_id < 8))
		{
			block.push_back(&chamber_wires);
		}
	}

	tracks[event_id] = prop_recognize_all_tracks(block);

	event_id++;

	if (event_id >= LOAD_EVENTS_COUNT)
	{
		throw 7;
	}
}
