#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "geometry.hpp"

using namespace std;

boost::regex	plane_property_regexp("//[[:space:]]?F([0-9])(\\*|X|Y)([0-9])\\.([[:word:]]+)[[:space:]]?=[[:space:]]?([^[:space:]]+)");

bool	Geometry::parse_plane_property_comment( string &line )
{
	group_id_t	group_id;
	bool		is_group_property;
	device_axis_t	axis;
	plane_id_t	plane_id;
	string		param_name;
	string		param_value;
	boost::smatch	mt;

	if (boost::regex_match(line, mt, plane_property_regexp))
	{
		auto	&axis_text = mt[2];
		group_id	= boost::lexical_cast<int>(mt[1]);
		plane_id	= boost::lexical_cast<int>(mt[3]);
		param_name	= mt[4];
		param_value	= mt[5];

		is_group_property = (axis_text == "*");

		if (!is_group_property)
		{
			if (axis_text == "X")
			{
				axis = DEV_AXIS_X;
			}
			else if (axis_text == "Y")
			{
				axis = DEV_AXIS_Y;
			}
			else
			{
				throw "Unknown axis type!";
			}
		}

		if (is_group_property && (param_name == "normal_pos"))
		{
			group[group_id][plane_id].normal_pos = boost::lexical_cast<double>(param_value);
		}
		else if ((!is_group_property) && (param_name == "axial_shift"))
		{
			plane_shifts[group_id][axis][plane_id] = boost::lexical_cast<double>(param_value);
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

	device.push_back(dev_props);

	return true;
}

void	Geometry::fill_arrays()
{
	device_id_t	device_id = 0;

	for(device_props_t &dev : device)
	{
		if (dev.chamber_id != INVALID_CHAMBER_ID)
		{
			// check if there is no plane information associated with
			// this (group_id, plane_id) pair

			if ((group.count(dev.group_id) == 0)
			    || (group[dev.group_id].count(dev.plane_id) == 0))
			{
				// if we dont have it, give warning and move to next device

				cerr << "Warning: no plane information for F" 
				     << int(dev.group_id) << "*" << int(dev.plane_id)
				     << " (device_id = " << device_id << ")" << endl;

				continue;
			}

			// otherwise, we will have use of a pointer to it

			plane_props_t*	plane = &(group[dev.group_id][dev.plane_id]);

			// so if there is no dev.chamber_id in our array

			vector<plane_props_t*>	&planes = group_planes[dev.group_id][dev.axis];
			vector<double>	&normal_pos = group_normal_pos[dev.group_id][dev.axis];
			vector<chamber_id_t>	&chambers = group_chambers[dev.group_id][dev.axis];

			dev.axial_shift = plane_shifts[dev.group_id][dev.axis][dev.plane_id];

			if (find(chambers.begin(), chambers.end(), dev.chamber_id) == chambers.end())
			{
				// insert our pointer and chamber_id pair into arrays

				planes.push_back(plane);
				normal_pos.push_back(plane->normal_pos);
				chambers.push_back(dev.chamber_id);
				chamber_plane[dev.chamber_id] = dev.plane_id;
				chamber_axial_shift[dev.chamber_id] = dev.axial_shift;
			}
		}

		device_id++;
	}
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

	fill_arrays();
}

chamber_id_t	Geometry::get_device_chamber( device_id_t device_id )
{
	return device[device_id].chamber_id;
}

wire_pos_t	Geometry::get_wire_pos( device_id_t device_id, wire_id_t wire_id )
{
	device_props_t	*p = &device[device_id];

	return p->shift + p->step * wire_id + p->axial_shift;
}
