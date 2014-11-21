#ifndef __DRIFTCALIBHOOK_HPP
#define __DRIFTCALIBHOOK_HPP

#include <epecur/cxx11_compat.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/StdHits.hpp>

static const int	MAX_TIME_COUNTS = 384;

class DriftCalibHook : public StdHits
{
private:

	unsigned int	generate_calibration_curve( chamber_id_t chamber_id );

public:

	map< chamber_id_t, vector<unsigned int> >	time_distributions;
	StdHits::calibration_curve_t	calibration_curve;

	DriftCalibHook( Geometry &g );
	void	generate_calibration_curves();
	virtual void	handle_event_end() override;
};

#endif
