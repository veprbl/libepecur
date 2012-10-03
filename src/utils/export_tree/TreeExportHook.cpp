#include <cstring>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "TreeExportHook.hpp"

TreeExportHook::TreeExportHook( Geometry &g, double max_chisq )
	: TrackRecognitionHook(g, max_chisq),
	  tree("tracks", "recognized tracks"),
	  drift_tree("drift", "drift chamber raw data")
{
	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
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
				store_name(group_name + "_track_count/i")
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

	drift_tree.Branch(
		"chamber_id",
		&stored_drift.chamber_id,
		"chamber_id/" CHAMBER_ID_ROOT_TYPE
		);
	drift_tree.Branch(
		"num_wires",
		&stored_drift.num_wires,
		"num_wires/i"
		);
	stored_drift.wire_pos_br = drift_tree.Branch(
		"wire_pos",
		nullptr,
		"wire_pos[num_wires]/" WIRE_POS_ROOT_TYPE
		);
	stored_drift.time_br = drift_tree.Branch(
		"time",
		nullptr,
		"time[num_wires]/" DRIFT_TIME_ROOT_TYPE
		);
}

TreeExportHook::~TreeExportHook()
{
	BOOST_FOREACH(auto ptr, names)
	{
		delete[] ptr;
	}
}

const char*	TreeExportHook::store_name( string name )
{
	names.push_back(new char[name.size()+1]);

	strcpy(names.back(), name.c_str());

	return names.back();
}

void	TreeExportHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
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

			BOOST_FOREACH(track_info_t &track, tracks)
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

	wire_pos_t	wire_pos;
	uint16_t	time;

	BOOST_FOREACH(auto &pair0, last_event_drift)
	{
		stored_drift.chamber_id = pair0.first;
		stored_drift.num_wires = pair0.second.size();
		stored_drift.wire_pos.clear();
		stored_drift.time.clear();

		BOOST_FOREACH(auto &pair1, pair0.second)
		{
			tie(wire_pos, time) = pair1;

			stored_drift.wire_pos.push_back(wire_pos);
			stored_drift.time.push_back(time);
		}

		stored_drift.wire_pos_br->SetAddress(stored_drift.wire_pos.data());
		stored_drift.time_br->SetAddress(stored_drift.time.data());

		drift_tree.Fill();
	}
}
