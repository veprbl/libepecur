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

#include "StdHits.hpp"

using boost::unordered_map;

class	TrackRecognitionHook: public StdHits
{
public:

	Geometry	geom;
	unordered_map< group_id_t, map< device_axis_t, vector<track_info_t> > >	last_tracks;

	TrackRecognitionHook(Geometry &g, StdHits::calibration_curve_t *c = NULL);

	virtual void	handle_event_start() override;
	virtual void	handle_event_end() override;
};


#endif
