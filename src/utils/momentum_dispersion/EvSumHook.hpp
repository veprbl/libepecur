#ifndef __EVSUMHOOK_HPP
#define __EVSUMHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

const chamber_id_t	MAX_CHAMBER_ID = 16;
const wire_id_t		WIRES_COUNT = 200;

class	EvSumHook: public LoadHook
{
private:
	Geometry	geom;

public:

	uint	chambers[MAX_CHAMBER_ID][WIRES_COUNT];

	EvSumHook( Geometry &g );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
};

#endif
