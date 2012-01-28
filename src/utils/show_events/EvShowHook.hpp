#ifndef __EVSHOWHOOK_HPP
#define __EVSHOWHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/track.hpp>
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
	unordered_map< chamber_id_t, vector<wire_pos_t> >	events[LOAD_EVENTS_COUNT];
	vector<track_info_t>				tracks[LOAD_EVENTS_COUNT];

	EvShowHook( Geometry &g );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
	virtual void	handle_event_end();
};

#endif
