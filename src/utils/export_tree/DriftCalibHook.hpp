#ifndef __DRIFTCALIBHOOK_HPP
#define __DRIFTCALIBHOOK_HPP

#include <epecur/cxx11_compat.hpp>
#include <epecur/loadfile.hpp>

static const int	MAX_TIME_COUNTS = 384;

class DriftCalibHook : public StdDrift
{
public:

	map< chamber_id_t, vector<unsigned int> >	time_distributions;

	DriftCalibHook( Geometry &g );
	~DriftCalibHook();
	void	handle_drift_data(
		std::vector<wire_id_t> &wire_id_s,
		std::vector<uint16_t> &time_s,
		device_id_t dev_id
		) override;
	virtual void	handle_event_end() override;
};

#endif
