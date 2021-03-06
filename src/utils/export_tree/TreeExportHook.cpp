#include <cstring>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/track.hpp>

#include "TreeExportHook.hpp"

TreeExportHook::TreeExportHook( Geometry &g, StdHits::calibration_curve_t *c )
	: TrackReconstructionHook(g, c),
	  event_tree(
		  "events",
		  "reconstructd tracks for prop chambers"
		  "and triggered drift wires"
		  ),
	  target_info_tree("target_info", ""),
	  cycle_efficiency_tree(
		  "cycle_efficiency",
		  ""
		  ),
	  event_id(0),
	  cycle_first_event_id(0),
	  cycle_all_count(0),
	  cycle_hit_count(0)
{
	event_tree.AddFriend("cycle_efficiency");
	event_tree.Branch(
		"event_cause",
		&event_info.event_cause,
		"event_cause/i"
		);
	event_tree.Branch(
		"timestamp",
		&event_info.timestamp,
		"timestamp/i"
		);

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

				cycle_efficiency_tree.Branch(
					store_name(group_name + "_cycle_efficiency"),
					&cycle_efficiency[group_id][axis],
					store_name(group_name + "_cycle_efficiency/D")
					);
			}
		}
	}

	target_info_tree.Branch(
		"target_info",
		(void*)NULL,
		"R[8]/D:V[7]/D:H[8]/D:manual/I:time/I"
		);
	cycle_efficiency_tree.Branch(
		"min_cycle_efficiency",
		&min_cycle_efficiency,
		"min_cycle_efficiency/D"
		);
}

TreeExportHook::~TreeExportHook()
{
	BOOST_FOREACH(auto ptr, names)
	{
		delete[] ptr;
	}
}

void	TreeExportHook::Write()
{
	if (cycle_all_count != 0)
	{
		std::cerr << "Warning: Last cycle didn't end properly" << std::endl;
		write_cycle_efficiencies();
	}

	event_tree.Write();
	target_info_tree.Write();
	cycle_efficiency_tree.Write();
}

void	TreeExportHook::init_track_group(
	string group_name, group_id_t group_id, device_axis_t axis
	)
{
	TrackGroup	&st_gr = stored_track[group_id][axis];
	group_name = "t" + group_name + "_";

	event_tree.Branch(
		store_name(group_name + "track_count"),
		&st_gr.track_count,
		store_name(group_name + "track_count/i")
		);
	event_tree.Branch(
		store_name(group_name + "c0"),
		"vector<double>",
		&st_gr.c0_ptr
		);
	event_tree.Branch(
		store_name(group_name + "c1"),
		"vector<double>",
		&st_gr.c1_ptr
		);
	event_tree.Branch(
		store_name(group_name + "hits_count"),
		"vector<unsigned int>",
		&st_gr.hits_count_ptr
		);
	event_tree.Branch(
		store_name(group_name + "chisq"),
		"vector<double>",
		&st_gr.chisq_ptr
		);
	event_tree.Branch(
		store_name(group_name + "prev_chisq"),
		"vector<double>",
		&st_gr.prev_chisq_ptr
		);
	event_tree.Branch(
		store_name(group_name + "used_chambers_mask"),
		"vector<unsigned int>",
		&st_gr.used_chambers_mask_ptr
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
		event_tree.Branch(
			store_name(group_name + "wire_pos"),
			"vector<double>",
			&st_gr.wire_pos_ptr
			);
		event_tree.Branch(
			store_name(group_name + "time"),
			"vector<UShort_t>",
			&st_gr.time_ptr
			);
		event_tree.Branch(
			store_name(group_name + "drift_offset"),
			"vector<double>",
			&st_gr.drift_offset_ptr
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
	TrackGroup	&st_gr = stored_track[group_id][axis];

	st_gr.track_count = tracks.size();
	st_gr.c0.clear();
	st_gr.c1.clear();
	st_gr.hits_count.clear();
	st_gr.chisq.clear();
	st_gr.prev_chisq.clear();
	st_gr.used_chambers_mask.clear();

	BOOST_FOREACH(track_info_t &track, tracks)
	{
		st_gr.c0.push_back(track.c0);
		st_gr.c1.push_back(track.c1);
		st_gr.hits_count.push_back(track.used_chambers.size());
		st_gr.chisq.push_back(track.chisq);
		st_gr.prev_chisq.push_back(track.prev_chisq);

		uint32_t mask = 0;
		BOOST_FOREACH(uint chamber_index, track.used_chambers)
		{
			mask |= (1 << chamber_index);
		}
		st_gr.used_chambers_mask.push_back(mask);
	}
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
		vector<wire_pos_t>	&drift_offset =
			last_event_drift_offset[chamber_id];

		BOOST_ASSERT(time.size() == wire_pos.size());
		BOOST_ASSERT(drift_offset.size() == wire_pos.size());

		st_gr.num_wires = wire_pos.size();
		st_gr.wire_pos_ptr = &wire_pos;
		st_gr.time_ptr = &time;
		st_gr.drift_offset_ptr = &drift_offset;
	}
}

/**
 * Write cycle efficiencies for each event of the last cycle.
 */
void	TreeExportHook::write_cycle_efficiencies()
{
	min_cycle_efficiency = -1;

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type == DEV_TYPE_DRIFT)
		{
			for(int i = 0; i != DEV_AXIS_END; i++)
			{
				device_axis_t	axis = static_cast<device_axis_t>(i);
				double cycle_efficiency_value =
					cycle_hit_count[group_id][axis] / (float)cycle_all_count;
				cycle_efficiency[group_id][axis] = cycle_efficiency_value;
				if ((min_cycle_efficiency < 0) || (cycle_efficiency_value < min_cycle_efficiency))
				{
					min_cycle_efficiency = cycle_efficiency_value;
				}
			}
		}
	}
	for(uint64_t j = cycle_first_event_id; j < event_id; j++)
	{
		cycle_efficiency_tree.Fill();
	}

	cycle_first_event_id = event_id;
	cycle_all_count = 0;
	cycle_hit_count.clear();
}

void	TreeExportHook::handle_timestamp( int32_t timestamp )
{
	event_info.timestamp = timestamp;
}

void	TreeExportHook::handle_trig_end_cycle()
{
	cycle_end_event_flag = true;
}

void	TreeExportHook::handle_trig_info(
	uint8_t devices_mask,
	uint16_t event_cause,
	uint32_t gate_time
	)
{
	event_info.event_cause = event_cause;
}

void	TreeExportHook::handle_event_start()
{
	TrackReconstructionHook::handle_event_start();

	cycle_end_event_flag = false;
}

void	TreeExportHook::handle_event_end()
{
	TrackReconstructionHook::handle_event_end();

	cycle_all_count++;

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type == DEV_TYPE_DRIFT)
		{
			for(int i = 0; i != DEV_AXIS_END; i++)
			{
				device_axis_t	axis = static_cast<device_axis_t>(i);
				vector<track_info_t>	&tracks = last_tracks[group_id][axis];
				BOOST_FOREACH(track_info_t &track, tracks)
				{
					if (track.used_chambers.size() == 4)
					{
						cycle_hit_count[group_id][axis]++;
						break;
					}
				}
			}
		}

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
	event_id++;

	if (cycle_end_event_flag)
	{
		write_cycle_efficiencies();
	}
}

void	TreeExportHook::handle_slow_target_info(
	const slow_target_record_t *rec
	)
{
	static TBranch	*br = target_info_tree.GetBranch("target_info");

	br->SetAddress((void*)rec);
	target_info_tree.Fill();
}
