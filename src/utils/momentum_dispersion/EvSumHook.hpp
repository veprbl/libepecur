#ifndef __EVSUMHOOK_HPP
#define __EVSUMHOOK_HPP

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>

const int	MAX_PLANE_ID = 16;
const int	WIRES_COUNT = 200;

class	EvSumHook: public LoadHook
{
private:
	Geometry	geom;

public:

	uint	planes[MAX_PLANE_ID][WIRES_COUNT];

	EvSumHook( Geometry &g );

	virtual void	handle_prop_data( const char* begin, const char* end, uint16_t dev_id, void* data );
};

#endif
