#include <cstring>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "TreeExportHook.hpp"

TreeExportHook::TreeExportHook( Geometry &g, double max_chisq )
	: TrackRecognitionHook(g, max_chisq), tree("tracks", "recognized tracks")
{
	for(auto gr_tup : geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			stored_group_t	&st_gr = stored_group[group_id][axis];

			st_gr.track_count = 0;

			string	group_name;
			stringstream	group_name_stream;
			group_name_stream << "g" << int(group_id) << ((axis == DEV_AXIS_X) ? "X" : "Y");
			group_name_stream >> group_name;

			tree.Branch(
				store_name(group_name + "_track_count"),
				&st_gr.track_count,
				store_name(group_name + "_track_count/I")
				);
			st_gr.c0_br = tree.Branch(
				store_name(group_name + "_c0"),
				nullptr,
				store_name(group_name + "_c0[" + group_name + "_track_count]/D")
				);
			st_gr.c1_br = tree.Branch(
				store_name(group_name + "_c1"),
				nullptr,
				store_name(group_name + "_c1[" + group_name + "_track_count]/D")
				);
			st_gr.hits_count_br = tree.Branch(
				store_name(group_name + "_hits_count"),
				nullptr,
				store_name(group_name + "_hits_count[" + group_name + "_track_count]/I")
				);
			st_gr.chisq_br = tree.Branch(
				store_name(group_name + "_chisq"),
				nullptr,
				store_name(group_name + "_chisq[" + group_name + "_track_count]/D")
				);
		}
	}
}

const char*	TreeExportHook::store_name( string name )
{
	names.push_back(unique_ptr< char[] >(new char[name.size()+1]));

	strcpy(names.back().get(), name.c_str());

	return names.back().get();
}

void	TreeExportHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	for(auto gr_tup : geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
//			vector<double>	&normal_pos = geom.group_normal_pos[group_id][axis];
//			int	track_count = last_tracks[group_id][axis].size();

			vector<track_info_t>	&tracks = last_tracks[group_id][axis];
			stored_group_t	&st_gr = stored_group[group_id][axis];

			st_gr.track_count = tracks.size();
			st_gr.c0.clear();
			st_gr.c1.clear();
			st_gr.hits_count.clear();
			st_gr.chisq.clear();

			for(track_info_t &track : tracks)
			{
				st_gr.c0.push_back(track.c0);
				st_gr.c1.push_back(track.c1);
				st_gr.hits_count.push_back(track.used_chambers.size());
				st_gr.chisq.push_back(track.chisq);
			}

			st_gr.c0_br->SetAddress(st_gr.c0.data());
			st_gr.c1_br->SetAddress(st_gr.c1.data());
			st_gr.hits_count_br->SetAddress(st_gr.hits_count.data());
			st_gr.chisq_br->SetAddress(st_gr.chisq.data());
		}
	}

	tree.Fill();
}
