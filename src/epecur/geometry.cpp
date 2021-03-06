#include <string>
#include <iostream>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "geometry.hpp"
#include "loadfile.hpp"

using namespace std;

inline device_type_t	get_device_type( device_id_t dev_id )
{
	if ((dev_id >= 0) && (dev_id < 100))
	{
		return DEV_TYPE_PROP;
	}
	else if ((dev_id >= 100) && (dev_id < 200))
	{
		return DEV_TYPE_DRIFT;
	}
	else
	{
		throw "Can't determine device type";
	}
}

boost::regex	group_property_regexp("//[[:space:]]?F([0-9])\\.([[:word:]]+)[[:space:]]?=[[:space:]]?([^[:space:]]+)");

bool	Geometry::parse_group_property_comment( string &line )
{
	boost::smatch	mt;

	if (boost::regex_match(line, mt, group_property_regexp))
	{
		group_id_t	group_id	= boost::lexical_cast<int>(mt[1]);
		string		param_name	= mt[2];
		string		param_value	= mt[3];

		if (param_name == "max_chisq")
		{
			group_max_chisq[group_id] = boost::lexical_cast<double>(param_value);
		}
		else
		{
			throw "Unknown group param name!";
		}

		return true;
	}

	return false;
}

boost::regex	plane_property_regexp("//[[:space:]]?F([0-9])(\\*|X|Y)([0-9])\\.([[:word:]]+)[[:space:]]?=[[:space:]]?([^[:space:]].+)");

bool	Geometry::parse_plane_property_comment( string &line )
{
	group_id_t	group_id;
	vector<device_axis_t>	axis;
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

		if (axis_text == "X")
		{
			axis.push_back(DEV_AXIS_X);
		}
		else if (axis_text == "Y")
		{
			axis.push_back(DEV_AXIS_Y);
		}
		else if (axis_text == "*")
		{
			axis.push_back(DEV_AXIS_X);
			axis.push_back(DEV_AXIS_Y);
		}
		else
		{
			throw "Unknown axis!";
		}

		if (param_name == "normal_pos")
		{
			BOOST_FOREACH(device_axis_t dev_axis, axis)
			{
				auto	&np = normal_pos[group_id][dev_axis];

				if (np.size() < plane_id)
				{
					np.resize(plane_id);
				}

				np[plane_id - 1] =
					boost::lexical_cast<double>(param_value);
			}
		}
		else if (param_name == "axial_shift")
		{
			if (axis.size() != 1)
			{
				throw "Unknown axis!";
			}

			double	axial_shift;

			if (param_value.find('/') == string::npos)
			{
				axial_shift = boost::lexical_cast<double>(param_value);
			}
			else
			{
				vector<string>	frac;
				boost::replace_all(param_value, " ", "");
				boost::replace_all(param_value, "\t", "");
				boost::split(frac, param_value, boost::is_any_of("/"));

				if (frac.size() != 2)
				{
					throw "Invalid fraction";
				}

				axial_shift = boost::lexical_cast<double>(frac[0]) /
					boost::lexical_cast<double>(frac[1]);
			}

			plane_shifts[group_id][axis.front()][plane_id] = axial_shift;
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
	auto	comment = line.find('/');

	if (begin == string::npos || end == string::npos || end < begin)
	{
		return false;
	}

	// Ignore commented lines
	if (comment != string::npos && comment < end)
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

	BOOST_FOREACH(device_props_t &dev, device)
	{
		if (dev.chamber_id != INVALID_CHAMBER_ID)
		{
			device_type_t	device_type =
				get_device_type(device_id);

			if (normal_pos[dev.group_id][dev.axis].empty())
			{
				cerr << "Warning: no normal_pos information for F" 
				     << int(dev.group_id) << "*" << int(dev.plane_id)
				     << " (device_id = " << device_id << ")" << endl;

				continue;
			}

			// so if there is no dev.chamber_id in our array

			vector<chamber_id_t>	&chambers = group_chambers[dev.group_id][dev.axis];

			dev.axial_shift = plane_shifts[dev.group_id][dev.axis][dev.plane_id];

			if (group_device_type.count(dev.group_id))
			{
				if (group_device_type[dev.group_id]
				    != device_type)
				{
					cerr << "Warning: inhomogenious group "
						"devices" << endl;
				}
			}
			else
			{
				group_device_type[dev.group_id] = device_type;
			}

			if (find(chambers.begin(), chambers.end(), dev.chamber_id) == chambers.end())
			{
				// insert our pointer and chamber_id pair into arrays

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

		if (parse_group_property_comment(line))
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
	chamber_id_t	chamber_id;

	try
	{
		chamber_id = device.at(device_id).chamber_id;
	}
	catch( out_of_range &e )
	{
		cerr << "out of range " << int(device_id) << endl;
		throw;
	}

	if (chamber_id == static_cast<chamber_id_t>(-1))
	{
		cerr << "Unknown chamber_id for device " << device_id << endl;
	}

	return chamber_id;
}

wire_pos_t	Geometry::get_wire_pos( device_id_t device_id, wire_id_t wire_id )
{
	device_props_t	*p = &device[device_id];

	return p->shift + p->step * wire_id + p->axial_shift;
}
