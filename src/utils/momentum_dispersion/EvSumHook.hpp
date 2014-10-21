#ifndef __EVSUMHOOK_HPP
#define __EVSUMHOOK_HPP

#include <set>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

#include <TH1F.h>

const chamber_id_t	MAX_CHAMBER_ID = 16;
const wire_id_t		WIRES_COUNT = 200;

class	EvSumHook: public LoadHook
{
private:
	Geometry	geom;
	std::set<int>	chambers;
	double		F1pos;

public:

	TH1F		h1, h2, h3, h4;

	EvSumHook( Geometry &g );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
	virtual void	handle_drift_data(std::vector<wire_id_t> &wire_id, std::vector<uint16_t> &time, device_id_t dev_id);
	virtual void	handle_event_end();
};

#endif
