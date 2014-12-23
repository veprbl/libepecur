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

#include "StdDrift.hpp"

using boost::unordered_map;

class	TrackRecognitionHook: public StdDrift
{
public:

	Geometry	geom;
	StdDrift::calibration_curve_t	*calibration_curve;
	unordered_map< chamber_id_t, vector<wire_pos_t> >	last_event;
	unordered_map< group_id_t, map< device_axis_t, vector<track_info_t> > >	last_tracks;

	TrackRecognitionHook( Geometry &g, StdDrift::calibration_curve_t *c = NULL );
	void	fill_drift_event();

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id ) override;
	virtual void	handle_event_start() override;
	virtual void	handle_event_end() override;
};


#endif
