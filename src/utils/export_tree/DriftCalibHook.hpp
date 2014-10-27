#ifndef __DRIFTCALIBHOOK_HPP
#define __DRIFTCALIBHOOK_HPP

#include <TTree.h>
#include <TH1F.h>

#include <epecur/cxx11_compat.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/StdDrift.hpp>

static const int	MAX_TIME_COUNTS = 384;

class DriftCalibHook : public StdDrift
{
private:

	TTree	drift_calib;
	TH1F	calib_curve;

	unsigned int	generate_calibration_curve( chamber_id_t chamber_id );

public:

	map< chamber_id_t, vector<unsigned int> >	time_distributions;
	StdDrift::calibration_curve_t	calibration_curve;

	DriftCalibHook( Geometry &g );
	void	generate_calibration_curves();
	virtual void	handle_event_end() override;
};

#endif
