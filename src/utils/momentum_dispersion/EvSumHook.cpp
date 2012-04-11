#include <cstring>
#include <cmath>

#include <epecur/types.hpp>

#include "EvSumHook.hpp"

EvSumHook::EvSumHook( Geometry &g ) : geom(g)
{
	memset(chambers, 0, sizeof(chambers));
}

void	EvSumHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);
		int	bin = int(floor(wire_pos)) + WIRES_COUNT / 2;

		if ((bin < 0) || (bin >= WIRES_COUNT))
		{
			continue;
		}

		chambers[chamber_id][bin] += 1;
	}
}
