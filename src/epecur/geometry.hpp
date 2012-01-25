#ifndef __GEOMETRY_HPP
#define __GEOMETRY_HPP

#include <vector>
#include <istream>

#include "types.hpp"

using namespace std;

struct device_props_t
{
	plane_id_t	plane;
	wire_pos_t	step;
	wire_pos_t	shift;
};

class Geometry
{
	vector<device_props_t>	props;
public:
	Geometry( istream& );
	wire_pos_t	get_wire_pos( int, char );
	plane_id_t	get_device_plane( int );
};

#endif
