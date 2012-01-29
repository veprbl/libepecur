#ifndef __TRACKRECOGNITION_HPP
#define __TRACKRECOGNITION_HPP

#include "geometry.hpp"
#include "loadfile.hpp"
#include "track.hpp"
#include "types.hpp"

#include <vector>
#include <map>
#include <unordered_map>

class	TrackRecognitionHook: public LoadHook
{
private:

	bool		last_event_finished;
	double		max_chisq;

public:

	Geometry	geom;
	unordered_map< chamber_id_t, vector<wire_pos_t> >	last_event;
	unordered_map< group_id_t, map< device_axis_t, vector<track_info_t> > >	last_tracks;

	TrackRecognitionHook( Geometry &g, double max_chisq = -1 );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
	virtual void	handle_event_end();
};


#endif
