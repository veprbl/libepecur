#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "geometry.hpp"

using namespace std;

boost::regex	plane_property_regexp("//[[:space:]]?F([0-9])plane([0-9])\\.([[:word:]]+)[[:space:]]?=[[:space:]]?([^[:space:]]+)");

bool	Geometry::parse_plane_property_comment( string &line )
{
	group_id_t	group_id;
	plane_id_t	plane_id;
	string		param_name;
	string		param_value;
	boost::smatch	mt;

	if (boost::regex_match(line, mt, plane_property_regexp))
	{
		group_id	= boost::lexical_cast<int>(mt[1]);
		plane_id	= boost::lexical_cast<int>(mt[2]);
		param_name	= mt[3];
		param_value	= mt[4];

		pair<group_id_t, plane_id_t>	plane_index(group_id, plane_id);

		if (!plane.count(plane_index))
		{
			plane[plane_index] = plane_props_t();
		}

		if (param_name == "normal_pos")
		{
			plane[plane_index].normal_pos = boost::lexical_cast<double>(param_value);
		}
		else
		{
			throw "Unknown param type";
		}

		return true;
	}

	return false;
}


boost::regex	plane_relation_regexp("//[[:space:]]?F([0-9])([XY])([0-9])[[:space:]]?");

bool	Geometry::parse_plane_relation_comment( string &line, group_id_t &group, device_axis_t &axis, plane_id_t &plane )
{
	boost::smatch	mt;

	if (boost::regex_match(line, mt, plane_relation_regexp))
	{
		group	= boost::lexical_cast<int>(mt[1]);
		plane	= boost::lexical_cast<int>(mt[3]);

		if (mt[2] == "X")
		{
			axis = DEV_AXIS_X;
		}
		else if (mt[2] == "Y")
		{
			axis = DEV_AXIS_Y;
		}
		else
		{
			throw "Unknown axis type!";
		}

		return true;
	}

	return false;
}

bool	Geometry::parse_chamber_info_text( string &line, group_id_t current_group, device_axis_t current_axis, plane_id_t current_plane )
{
	static device_id_t	device_id = 0;

	auto	begin = line.find('{');
	auto	end = line.rfind('}');

	if (begin == string::npos || end == string::npos || end < begin)
	{
		return false;
	}

	string	sub = line.substr(begin + 1, end - begin - 1);

	boost::replace_all(sub, " ", "");

	vector<string>	value;
	boost::split(value, sub, boost::is_any_of(","));

	if (value.size() != 3)
	{
		throw "Tuple with wrong size in geometry file!";
	}

	device_props_t	dev_props;

	dev_props.chamber_id	= boost::lexical_cast<int>(value[0]);
	dev_props.step		= boost::lexical_cast<int>(value[1]);
	dev_props.shift		= boost::lexical_cast<int>(value[2]);
	dev_props.group_id	= current_group;
	dev_props.axis		= current_axis;
	dev_props.plane_id	= current_plane;

	pair<device_id_t, plane_id_t>	plane_index(dev_props.group_id, dev_props.plane_id);

	if (dev_props.chamber_id != INVALID_CHAMBER_ID)
	{
		if (plane.count(plane_index) == 0)
		{
			cerr << "Warning: no plane information for F" << int(dev_props.group_id) << "*" << int(dev_props.plane_id) <<
				" (device_id = " << device_id << ")" << endl;
		}
		else
		{
			dev_props.plane = &(plane[plane_index]);
		}
	}

	device.push_back(dev_props);

	device_id++;

	return true;
}

Geometry::Geometry( istream &in )
{
	group_id_t	current_group;
	device_axis_t	current_axis;
	plane_id_t	current_plane;
	string	line;


	while(getline(in, line))
	{
		if (parse_plane_relation_comment(
			    line,
			    current_group,
			    current_axis,
			    current_plane
			    ))
		{
			continue;
		}

		if (parse_plane_property_comment(line))
		{
			continue;
		}

		if (parse_chamber_info_text(
			    line,
			    current_group,
			    current_axis,
			    current_plane
			    ))
		{
			continue;
		}
	}
}

chamber_id_t	Geometry::get_device_chamber( device_id_t device_id )
{
	return device[device_id].chamber_id;
}

wire_pos_t	Geometry::get_wire_pos( device_id_t device_id, wire_id_t wire_id )
{
	device_props_t	*p = &device[device_id];

	return p->shift + p->step * wire_id;
}
