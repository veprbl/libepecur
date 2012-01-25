#include <cstring>

#include <epecur/types.hpp>

#include "EvSumHook.hpp"

EvSumHook::EvSumHook( Geometry &g ) : geom(g)
{
	memset(planes, 0, sizeof(planes));
}

void	EvSumHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, uint16_t dev_id )
{
	plane_id_t	plane_id = geom.get_device_plane(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		planes[plane_id][wire_pos + 100] += 1;
	}
}
