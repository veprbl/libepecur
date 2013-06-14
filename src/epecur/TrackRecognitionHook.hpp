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
#include <boost/optional.hpp>

#include "StdDrift.hpp"

using boost::unordered_map;

static const int	MAX_TIME_COUNTS = 384;
static const int	MAX_ANGLE_COUNTS = 180;
static const int	ANGLE_COUNTS_OFFSET = 90;
static const double	DRIFT_STEP = 17.0; // mm

class	TrackRecognitionHook: public StdDrift
{
private:

	bool		last_event_finished;

public:

	Geometry	geom;
	StdDrift::calibration_curve_t	*calibration_curve;
	unordered_map< chamber_id_t, vector<wire_pos_t> >	last_event;
	unordered_map< group_id_t, map< device_axis_t, vector<track_info_t> > >	last_tracks;

	TrackRecognitionHook( Geometry &g, StdDrift::calibration_curve_t *c = NULL );

	double	c1_to_angle( double c1 );
	boost::optional<track_info_t>	recognize_drift_track(
		const track_info_t	&rough_track,
		const vector<double>  &normal_pos,
		const vector<chamber_id_t>    &chambers,
		double	max_chisq
		);

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id ) override;
	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) override;
	virtual void	handle_event_end() override;
};


#endif
