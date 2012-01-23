#include <cstring>
#include <iostream>

#include "EvShowHook.hpp"

EvShowHook::EvShowHook( Geometry &g ) : geom(g)
{
	memset(wire_ids, 0, sizeof(wire_ids));
	memset(last_wire_place, 0, sizeof(last_wire_place));
}

void	EvShowHook::handle_prop_data( const char* begin, const char* end, uint16_t dev_id, void* data )
{
	plane_id_t	plane_id = geom.get_device_plane(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		int	wire_pos = geom.get_wire_pos(dev_id, *pos);
		auto	wire_place = (last_wire_place[event_id][plane_id]++);

		cerr << "event_id" << event_id << "  plane_id: " << plane_id << "  wire_pos: " << wire_pos << "  wire_place:" << wire_place << endl;

		if (wire_place > WIRES_COUNT)
		{
			cerr << "Wires overflow" << endl;
			continue;
		}

		wire_ids[event_id][plane_id][wire_place] = wire_pos + WIRES_COUNT / 2;
	}
}

void	EvShowHook::handle_event_end()
{
	event_id++;

	if (event_id >= LOAD_EVENTS_COUNT)
	{
		throw 7;
	}
}
