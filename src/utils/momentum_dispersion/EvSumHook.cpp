#include <cstring>

#include "EvSumHook.hpp"

EvSumHook::EvSumHook( Geometry &g ) : geom(g)
{
	memset(planes, 0, sizeof(planes));
}

void	EvSumHook::handle_prop_data( const char* begin, const char* end, uint16_t dev_id, void* data )
{
	plane_id_t	plane_id = geom.get_device_plane(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		int	wire_pos = geom.get_wire_pos(dev_id, *pos);

		planes[plane_id][wire_pos + 100] += 1;
	}
}
