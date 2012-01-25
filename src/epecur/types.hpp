#include <cstdint>
#include <boost/static_assert.hpp>

typedef	uint8_t	wire_id_t;
typedef	uint8_t	wire_id_ptr_t;

BOOST_STATIC_ASSERT(sizeof(wire_id_ptr_t) >= sizeof(wire_id_t));

typedef	int8_t	wire_pos_t;
typedef	uint8_t	wire_pos_ptr_t;

BOOST_STATIC_ASSERT(sizeof(wire_pos_ptr_t) >= sizeof(wire_pos_t));

typedef	uint8_t	plane_id_t;
