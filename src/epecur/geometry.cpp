#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "geometry.hpp"

using namespace std;

boost::regex	plane_info_regexp("//[[:space:]]?F([0-9])([XY])([0-9])[[:space:]]?");

bool	Geometry::parse_plane_info_comment( string &line, group_id_t &group, device_axis_t &axis, plane_id_t &plane )
{
	boost::smatch	mt;

	if (boost::regex_match(line, mt, plane_info_regexp))
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

bool	Geometry::parse_chamber_info_text( string &line, chamber_id_t &chamber, wire_id_t &step, wire_id_t &shift )
{
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

	chamber	= boost::lexical_cast<int>(value[0]);
	step	= boost::lexical_cast<int>(value[1]);
	shift	= boost::lexical_cast<int>(value[2]);

	return true;
}

Geometry::Geometry( istream &in )
{
	chamber_id_t	chamber;
	wire_id_t	step;
	wire_id_t	shift;
	group_id_t	current_group;
	device_axis_t	current_axis;
	plane_id_t	current_plane;
	string	line;


	while(getline(in, line))
	{
		if (parse_plane_info_comment(
			    line,
			    current_group,
			    current_axis,
			    current_plane
			    ))
		{
			continue;
		}


		if (parse_chamber_info_text(
			    line,
			    chamber,
			    step,
			    shift
			    ))
		{
			device_props_t	dev_props;

			dev_props.chamber	= chamber;
			dev_props.step		= step;
			dev_props.shift	= shift;
			dev_props.group	= current_group;
			dev_props.axis		= current_axis;
			dev_props.plane	= current_plane;

			props.push_back(dev_props);
		}
	}
}

chamber_id_t	Geometry::get_device_chamber( device_id_t device_id )
{
	return props[device_id].chamber;
}

wire_pos_t	Geometry::get_wire_pos( device_id_t device_id, wire_id_t wire_id )
{
	device_props_t	*p = &props[device_id];

	return p->shift + p->step * wire_id;
}
