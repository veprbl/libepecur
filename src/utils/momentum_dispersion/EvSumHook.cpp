#include <cstring>

#include <epecur/types.hpp>

#include "EvSumHook.hpp"

EvSumHook::EvSumHook( Geometry &g ) : geom(g)
{
	memset(chambers, 0, sizeof(chambers));
}

void	EvSumHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, uint16_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		chambers[chamber_id][wire_pos + 100] += 1;
	}
}
