#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "types.hpp"
#include "geometry.hpp"

using namespace std;

Geometry::Geometry( istream &in )
{
	string	line;

	while(getline(in, line))
	{
		auto	begin = line.find('{');
		auto	end = line.rfind('}');

		if (begin == string::npos || end == string::npos || end < begin)
		{
			continue;
		}

		string	sub = line.substr(begin + 1, end - begin - 1);

		boost::replace_all(sub, " ", "");

		vector<string>	value;
		boost::split(value, sub, boost::is_any_of(","));

		if (value.size() != 3)
		{
			throw "Tuple with wrong size in geometry file!";
		}

		device_props_t*	dev_props = new device_props_t();

		dev_props->plane	= boost::lexical_cast<int>(value[0]);
		dev_props->step		= boost::lexical_cast<int>(value[1]);
		dev_props->shift	= boost::lexical_cast<int>(value[2]);

		props.push_back(*dev_props);
	}
}

plane_id_t	Geometry::get_device_plane( int device_id )
{
	return props[device_id].plane;
}

wire_pos_t	Geometry::get_wire_pos( int device_id, char wire_id )
{
	device_props_t	*p = &props[device_id];

	return p->shift + p->step * wire_id;
}
