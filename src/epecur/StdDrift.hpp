#ifndef __STDDRIFT_HPP
#define __STDDRIFT_HPP

#include <vector>
#include <map>

#include "cxx11_compat.hpp"
#include "geometry.hpp"
#include "loadfile.hpp"
#include "types.hpp"

using namespace std;

class	StdDrift: public LoadHook
{
protected:

	Geometry	geom;

public:

	typedef map< chamber_id_t, vector<double> >	calibration_curve_t;

	map< chamber_id_t, vector<wire_pos_t> >	last_event_drift_wire_pos;
	map< chamber_id_t, vector<uint16_t> >	last_event_drift_time;

	StdDrift(Geometry &g) : geom(g) {};
	~StdDrift() {};

	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) override;
	virtual void	handle_event_start() override;
};

#endif
