#ifndef __EVSHOWHOOK_HPP
#define __EVSHOWHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>

const uint	LOAD_EVENTS_COUNT = 10;
const uint	MAX_PLANE_ID = 16;
const uint	WIRES_COUNT = 200;

class	EvShowHook: public LoadHook
{
private:
	Geometry	geom;

public:

	uint	event_id;
	uint	last_wire_place[LOAD_EVENTS_COUNT][MAX_PLANE_ID];
	char	wire_ids[LOAD_EVENTS_COUNT][MAX_PLANE_ID][WIRES_COUNT];

	EvShowHook( Geometry &g );

	virtual void	handle_prop_data( const char* begin, const char* end, uint16_t dev_id );
	virtual void	handle_event_end();
};

#endif
