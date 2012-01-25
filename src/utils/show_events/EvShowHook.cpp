#include <cstring>
#include <iostream>

#include <epecur/types.hpp>

#include "EvShowHook.hpp"

EvShowHook::EvShowHook( Geometry &g ) : geom(g)
{
	// nothing
}

void	EvShowHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, uint16_t dev_id )
{
	auto	&event = events[event_id];
	plane_id_t	plane_id = geom.get_device_plane(dev_id);

	if (event.count(plane_id) == 0)
	{
		event[plane_id] = vector<wire_pos_t>();
	}

	auto &plane = event[plane_id];

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		plane.push_back(wire_pos);
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
