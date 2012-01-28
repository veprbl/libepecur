#ifndef __EVSHOWHOOK_HPP
#define __EVSHOWHOOK_HPP

#include <epecur/TrackRecognitionHook.hpp>

#include <vector>
#include <unordered_map>

const uint	LOAD_EVENTS_COUNT = 10;

class EvShowHook : public TrackRecognitionHook
{
private:

	uint	event_id;

public:

	unordered_map< chamber_id_t, vector<wire_pos_t> >	events[LOAD_EVENTS_COUNT];
	vector<track_info_t>				tracks[LOAD_EVENTS_COUNT];

	EvShowHook( Geometry &g );
	virtual void	handle_event_end();
};

#endif
