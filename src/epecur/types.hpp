#ifndef __TYPES_HPP
#define __TYPES_HPP

#include <cstdint>
#include <boost/static_assert.hpp>

typedef	unsigned int	uint;

typedef	uint8_t	wire_id_t;
typedef	uint8_t	wire_id_ptr_t;

BOOST_STATIC_ASSERT(sizeof(wire_id_ptr_t) >= sizeof(wire_id_t));

typedef	double	wire_pos_t;
typedef	uint8_t	wire_pos_ptr_t;

typedef	uint8_t	plane_id_t;
typedef	uint8_t	chamber_id_t;
typedef uint8_t	group_id_t;
typedef uint16_t	device_id_t;

enum device_axis_t
{
	DEV_AXIS_X,
	DEV_AXIS_Y
};

#endif
