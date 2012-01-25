#ifndef __EVSHOWHOOK_HPP
#define __EVSHOWHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

#include <vector>
#include <unordered_map>

const uint	LOAD_EVENTS_COUNT = 10;

class	EvShowHook: public LoadHook
{
private:
	Geometry	geom;

public:

	uint	event_id;
	unordered_map< int, vector<wire_pos_t> >	events[LOAD_EVENTS_COUNT];

	EvShowHook( Geometry &g );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, uint16_t dev_id );
	virtual void	handle_event_end();
};

#endif
