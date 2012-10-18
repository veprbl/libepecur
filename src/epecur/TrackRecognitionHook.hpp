#ifndef __TRACKRECOGNITION_HPP
#define __TRACKRECOGNITION_HPP

#include "cxx11_compat.hpp"
#include "geometry.hpp"
#include "loadfile.hpp"
#include "track.hpp"
#include "types.hpp"

#include <vector>
#include <map>

#include <boost/unordered/unordered_map.hpp>

using boost::unordered_map;

static const int	MAX_TIME_COUNTS = 384;

class	TrackRecognitionHook: public LoadHook
{
private:

	bool		last_event_finished;
	bool		drift_cleanup;
	double		max_chisq;

public:

	Geometry	geom;
	unordered_map< chamber_id_t, vector<wire_pos_t> >	last_event;
	unordered_map< group_id_t, map< device_axis_t, vector<track_info_t> > >	last_tracks;

	// drift
	map< chamber_id_t, vector<wire_pos_t> >	last_event_drift_wire_pos;
	map< chamber_id_t, vector<uint16_t> >	last_event_drift_time;
	map< chamber_id_t, map< wire_pos_t, vector<unsigned int> > >	time_distributions;

	TrackRecognitionHook( Geometry &g, double max_chisq = -1 );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) override;
	virtual void	handle_event_end();
};


#endif
