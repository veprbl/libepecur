#ifndef __EVSHOWHOOK_HPP
#define __EVSHOWHOOK_HPP

#include <vector>

#include <TH1I.h>

#include <epecur/types.hpp>
#include <epecur/TrackRecognitionHook.hpp>

#include <boost/unordered/unordered_map.hpp>

using boost::unordered_map;

class AxialShiftHook : public TrackRecognitionHook
{
private:

	unordered_map< chamber_id_t, TH1I* >	&shift_hist;

public:

	unordered_map< chamber_id_t, uint >	track_miss_count;
	int	track_count;

	AxialShiftHook( Geometry &g, unordered_map< chamber_id_t, TH1I* > &shift_hist, double max_chisq = -1 );
	virtual void	handle_event_end();
};

#endif
