#include <cstring>
#include <algorithm>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <TH1I.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "AxialShiftHook.hpp"

AxialShiftHook::AxialShiftHook( Geometry &g, unordered_map< chamber_id_t, TH1I* > &shift_hist )
  : TrackRecognitionHook(g), shift_hist(shift_hist)
{
	// nothing
}

void	AxialShiftHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type != DEV_TYPE_PROP)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			track_count += last_tracks[group_id][axis].size();

			BOOST_FOREACH(track_info_t &track, last_tracks[group_id][axis])
			{
				for(uint chamber_id_pos = 0; chamber_id_pos < axis_tup.second.size(); chamber_id_pos++)
				{
					if (find(track.used_chambers.begin(), track.used_chambers.end(), chamber_id_pos) == track.used_chambers.end())
					{
						track_miss_count[axis_tup.second[chamber_id_pos]] += 1;
					}
				}

				int	i = 0;

				BOOST_FOREACH(uint chamber_id_pos, track.used_chambers)
				{
					chamber_id_t	chamber_id = axis_tup.second[chamber_id_pos];
					plane_id_t	plane_id = geom.chamber_plane[chamber_id];
					double shift;

					if ((group_id != 1) || (axis != DEV_AXIS_X))
					{
						continue;
					}

					shift = track.chamber_wires_pos[i] - (track.c0 + track.c1 * normal_pos[chamber_id_pos]);

					shift_hist[chamber_id]->Fill(shift);

					i++;
				}
			}
		}
	}

}
