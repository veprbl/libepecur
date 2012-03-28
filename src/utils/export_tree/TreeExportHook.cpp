#include <cstring>
#include <algorithm>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "TreeExportHook.hpp"

TreeExportHook::TreeExportHook( Geometry &g, double max_chisq )
	: TrackRecognitionHook(g, max_chisq), tree("tracks", "recognized tracks")
{
	tree.Branch("c0", &c0, "c0/D");
	tree.Branch("c1", &c1, "c1/D");
	tree.Branch("group_id", &group_id, "c1/B");
}

void	TreeExportHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	for(auto gr_tup : geom.group_chambers)
	{
		group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
//			vector<double>	&normal_pos = geom.group_normal_pos[group_id][axis];
//			int	track_count = last_tracks[group_id][axis].size();

			for(track_info_t &track : last_tracks[group_id][axis])
			{
				c0 = track.c0;
				c1 = track.c1;
				tree.Fill();
			}
		}
	}

}
