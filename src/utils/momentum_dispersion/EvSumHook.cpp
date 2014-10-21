#include <cstring>
#include <cmath>

#include <epecur/types.hpp>

#include "EvSumHook.hpp"

using namespace std;

EvSumHook::EvSumHook( Geometry &g ) : geom(g)
	, h1("h1", "", 200, -100, 100)
	, h2("h2", "", 200, -100, 100)
	, h3("h3", "", 200, -100, 100)
	, h4("h4", "", 200, -100, 100)
{
}

void	EvSumHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	chambers.insert(chamber_id);

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		//chambers[chamber_id][bin] += 1;
		if ((end == begin+1) && (chamber_id == 0))
		{
			if (std::isnan(F1pos))
			{
				F1pos = wire_pos;
			}
			else
			{
				F1pos = NAN;
			}
		}
	}
}

void	EvSumHook::handle_drift_data(std::vector<wire_id_t> &wire_id, std::vector<uint16_t> &time, device_id_t dev_id)
{
	chamber_id_t    chamber_id = geom.get_device_chamber(dev_id);

//	if ((chamber_id == 30) || (chamber_id == 31)) cout << ">> ";

	//cout << "drift: dev_id = " << (int)dev_id << "; chamber_id = " << (int)chamber_id << endl;
	chambers.insert(chamber_id);
}

void	EvSumHook::handle_event_end()
{
	if (chambers.count(0) && chambers.count(2) && chambers.count(4) && chambers.count(6))
	{
		if ((!chambers.count(30)) && chambers.count(31) && chambers.count(34) && chambers.count(35))
		{
			h1.Fill(F1pos);
		}
		if (chambers.count(30) && (!chambers.count(31)) && chambers.count(34) && chambers.count(35))
		{
			h2.Fill(F1pos);
		}
		if (chambers.count(30) && chambers.count(31) && (!chambers.count(34)) && chambers.count(35))
		{
			h3.Fill(F1pos);
		}
		if (chambers.count(30) && chambers.count(31) && chambers.count(34) && (!chambers.count(35)))
		{
			h4.Fill(F1pos);
		}
	}

	F1pos = NAN;
	chambers.clear();
}
