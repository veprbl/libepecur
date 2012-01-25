#ifndef __EVSUMHOOK_HPP
#define __EVSUMHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

const plane_id_t	MAX_PLANE_ID = 16;
const wire_id_t		WIRES_COUNT = 200;

class	EvSumHook: public LoadHook
{
private:
	Geometry	geom;

public:

	uint	planes[MAX_PLANE_ID][WIRES_COUNT];

	EvSumHook( Geometry &g );

	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, uint16_t dev_id );
};

#endif
