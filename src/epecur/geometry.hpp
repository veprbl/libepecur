#ifndef __GEOMETRY_HPP
#define __GEOMETRY_HPP

#include <vector>
#include <map>
#include <istream>

#include "types.hpp"

using namespace std;

struct device_props_t
{
	chamber_id_t	chamber;
	wire_pos_t	step;
	wire_pos_t	shift;
	group_id_t	group;
	device_axis_t	axis;
	plane_id_t	plane;
};

struct plane_props_t
{
	double	normal_pos;
};

class Geometry
{
private:

	vector<device_props_t>	device;
	map< pair<group_id_t, plane_id_t>, plane_props_t >	plane;
	bool	parse_plane_property_comment( string& );
	bool	parse_plane_relation_comment( string&, group_id_t&, device_axis_t&, plane_id_t& );
	bool	parse_chamber_info_text( string&, chamber_id_t&, wire_id_t&, wire_id_t& );
	bool	check();

public:

	Geometry( istream& );
	wire_pos_t	get_wire_pos( device_id_t, wire_id_t );
	chamber_id_t	get_device_chamber( device_id_t );
};

const chamber_id_t	INVALID_CHAMBER_ID = 255;

#endif
