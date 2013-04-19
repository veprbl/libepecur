#include <cstring>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "TreeExportHook.hpp"

TreeExportHook::TreeExportHook( Geometry &g, StdDrift::calibration_curve_t *c )
	: TrackRecognitionHook(g, c),
	  event_tree(
		  "events",
		  "recognized tracks for prop chambers"
		  "and triggered drift wires"
		  )
{
	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			string	group_name =
				boost::lexical_cast<string>(int(group_id)) +
				((axis == DEV_AXIS_X) ? "X" : "Y");

			if ((device_type == DEV_TYPE_PROP) || (device_type == DEV_TYPE_DRIFT))
			{
				init_track_group(group_name, group_id, axis);
			}
			if (device_type == DEV_TYPE_DRIFT)
			{
				init_drift_group(group_name, group_id, axis);
			}
		}
	}
}

TreeExportHook::~TreeExportHook()
{
	BOOST_FOREACH(auto ptr, names)
	{
		delete[] ptr;
	}
}

void	TreeExportHook::init_track_group(
	string group_name, group_id_t group_id, device_axis_t axis
	)
{
	trace_group_t	&st_gr = stored_trace[group_id][axis];
	group_name = "t" + group_name + "_";

	event_tree.Branch(
		store_name(group_name + "track_count"),
		&st_gr.track_count,
		store_name(group_name + "track_count/i")
		);
	st_gr.c0_br = event_tree.Branch(
		store_name(group_name + "c0"),
		nullptr,
		store_name(group_name + "c0[" + group_name + "track_count]/D")
		);
	st_gr.c1_br = event_tree.Branch(
		store_name(group_name + "c1"),
		nullptr,
		store_name(group_name + "c1[" + group_name + "track_count]/D")
		);
	st_gr.hits_count_br = event_tree.Branch(
		store_name(group_name + "hits_count"),
		nullptr,
		store_name(group_name + "hits_count[" + group_name + "track_count]/I")
		);
	st_gr.chisq_br = event_tree.Branch(
		store_name(group_name + "chisq"),
		nullptr,
		store_name(group_name + "chisq[" + group_name + "track_count]/D")
		);
}

void	TreeExportHook::init_drift_group(
	string _group_name, group_id_t group_id, device_axis_t axis
	)
{
	vector<chamber_id_t>	&chambers = geom.group_chambers[group_id][axis];
	int	i = 1;

	BOOST_FOREACH(chamber_id_t chamber_id, chambers)
	{
		drift_group_t	&st_gr =
			stored_drift[group_id][axis][chamber_id];
		string	group_name =
			"d" + _group_name +
			boost::lexical_cast<string>(i) + "_";

		event_tree.Branch(
			store_name(group_name + "num_wires"),
			&st_gr.num_wires,
			store_name(group_name + "num_wires/i")
			);
		st_gr.wire_pos_br = event_tree.Branch(
			store_name(group_name + "wire_pos"),
			nullptr,
			store_name(group_name + "wire_pos[" + group_name + "num_wires]/" WIRE_POS_ROOT_TYPE)
			);
		st_gr.time_br = event_tree.Branch(
			store_name(group_name + "time"),
			nullptr,
			store_name(group_name + "time[" + group_name + "num_wires]/" DRIFT_TIME_ROOT_TYPE)
			);

		++i;
	}
}

const char*	TreeExportHook::store_name( string name )
{
	names.push_back(new char[name.size()+1]);

	strcpy(names.back(), name.c_str());

	return names.back();
}

void	TreeExportHook::write_track_event(
	group_id_t group_id, device_axis_t axis
	)
{
	vector<track_info_t>	&tracks = last_tracks[group_id][axis];
	trace_group_t	&st_gr = stored_trace[group_id][axis];

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

void	TreeExportHook::write_drift_event(
	group_id_t group_id, device_axis_t axis
	)
{
	vector<chamber_id_t>	&chambers = geom.group_chambers[group_id][axis];

	BOOST_FOREACH(chamber_id_t chamber_id, chambers)
	{
		drift_group_t	&st_gr =
			stored_drift[group_id][axis][chamber_id];
		vector<wire_pos_t>      &wire_pos =
			last_event_drift_wire_pos[chamber_id];
		vector<uint16_t>        &time =
			last_event_drift_time[chamber_id];

		st_gr.num_wires = wire_pos.size();
		st_gr.wire_pos_br->SetAddress(wire_pos.data());
		st_gr.time_br->SetAddress(time.data());
	}
}

void	TreeExportHook::handle_event_end()
{
	TrackRecognitionHook::handle_event_end();

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			if ((device_type == DEV_TYPE_PROP) || (device_type == DEV_TYPE_DRIFT))
			{
				write_track_event(group_id, axis);
			}
			if (device_type == DEV_TYPE_DRIFT)
			{
				write_drift_event(group_id, axis);
			}
		}
	}

	event_tree.Fill();
}
