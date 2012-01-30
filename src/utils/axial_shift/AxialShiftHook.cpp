#include <cstring>
#include <algorithm>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <TH1I.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "AxialShiftHook.hpp"

AxialShiftHook::AxialShiftHook( Geometry &g, unordered_map< chamber_id_t, TH1I* > &shift_hist, double max_chisq )
  : TrackRecognitionHook(g, max_chisq), shift_hist(shift_hist)
{
	// nothing
}

void	AxialShiftHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	for(auto gr_tup : geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			vector<double>	&normal_pos = geom.group_normal_pos[group_id][axis];

			for(track_info_t &track : last_tracks[group_id][axis])
			{
				int	i = 0;

				for(chamber_id_t chamber_id : axis_tup.second)
				{
					double shift;

					shift = track.chamber_wires_pos[i] - (track.c0 + track.c1 * normal_pos[i]);

					shift_hist[chamber_id]->Fill(shift);

					i++;
				}
			}
		}
	}

}
