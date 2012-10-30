#include <cstring>
#include <algorithm>
#include <iostream>

#include <boost/foreach.hpp>

#include <epecur/types.hpp>

#include "EvShowHook.hpp"

EvShowHook::EvShowHook( Geometry &g ) : TrackRecognitionHook(g), event_id(0)
{
	// nothing
}

void	EvShowHook::handle_event_end()
{
	events[event_id] = last_event;

	TrackRecognitionHook::handle_event_end();

	tracks[event_id] = last_tracks[1][DEV_AXIS_X];

	vector<double>	&normal_pos = geom.normal_pos[1][DEV_AXIS_X];

	vector<double>::iterator	max_pos = max_element(normal_pos.begin(), normal_pos.end());
	vector<double>::iterator	min_pos = min_element(normal_pos.begin(), normal_pos.end());

	BOOST_FOREACH(track_info_t &track, tracks[event_id])
	{
		track.c1 *= *max_pos - *min_pos;
	}

	event_id++;

	if (event_id >= LOAD_EVENTS_COUNT)
	{
		throw 7;
	}
}
