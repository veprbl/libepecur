#include <cstdlib>
#include <cmath>
#include <bitset>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "TrackRecognitionHook.hpp"

TrackRecognitionHook::TrackRecognitionHook(
	Geometry &g,
	StdDrift::calibration_curve_t *c
	)
	: StdDrift(g),
	  last_event_finished(false),
	  geom(g),
	  calibration_curve(c)
{
	// nothing
}

void	TrackRecognitionHook::handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id )
{
	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	if (last_event_finished)
	{
		last_event.clear();

		BOOST_FOREACH(auto gr_tup, last_tracks)
		{
			BOOST_FOREACH(auto axis_tup, gr_tup.second)
			{
				axis_tup.second.clear();
			}

		}

		last_event_finished = false;
	}

	auto &chamber = last_event[chamber_id];
	wire_id_t	prev_pos;
	int	sequence_len = 1, max_sequence_len = 3;

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		if ((!chamber.empty()) && (prev_pos == (*pos) - 1))
		{
			sequence_len++;
		}
		else
		{
			if (sequence_len > max_sequence_len)
			{
				chamber.resize(chamber.size() - sequence_len);
			}
			sequence_len = 1;
		}

		chamber.push_back(wire_pos);
		BOOST_ASSERT(chamber.size() >= sequence_len);

		prev_pos = *pos;
	}

	if (sequence_len > max_sequence_len)
	{
		chamber.resize(chamber.size() - sequence_len);
	}
}

void	TrackRecognitionHook::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	StdDrift::handle_drift_data(wire_id_s, time_s, dev_id);

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);

	last_event[chamber_id] = last_event_drift_wire_pos[chamber_id];
}

double	TrackRecognitionHook::c1_to_angle( double c1 )
{
	small_angle_t	psi = round(atan(c1 * DRIFT_STEP / 2) / M_PI * 180);
	psi += ANGLE_COUNTS_OFFSET;

	BOOST_ASSERT(psi < MAX_ANGLE_COUNTS);
	BOOST_ASSERT(psi >= 0);

	return psi;
}

boost::optional<track_info_t>	TrackRecognitionHook::recognize_drift_track(
	const track_info_t	&rough_track,
	const vector<double>	&normal_pos,
	const vector<chamber_id_t>    &chambers,
	double	max_chisq
	)
{
	vector< vector<wire_pos_t>* >	block;
	auto	wire_index_it = rough_track.wire_pos_ptr.begin();
	small_angle_t	psi = c1_to_angle(rough_track.c1);

	BOOST_FOREACH(auto chamber_index, rough_track.used_chambers)
	{
		chamber_id_t	chamber_id = chambers[chamber_index];
		wire_pos_ptr_t	wire_index = *(wire_index_it++);
		auto	&wire_pos = last_event_drift_wire_pos[chamber_id][wire_index];
		auto	&time = last_event_drift_time[chamber_id][wire_index];
		auto	&calib = (*calibration_curve)[chamber_id][psi];

		last_event[chamber_id].clear();
		last_event[chamber_id].push_back(DRIFT_STEP/2*(wire_pos + calib[time]));
		last_event[chamber_id].push_back(DRIFT_STEP/2*(wire_pos - calib[time]));
		block.push_back(&last_event[chamber_id]);
	}

	vector<track_info_t>	tr = recognize_all_tracks<track_type_t::drift>(block, normal_pos, max_chisq);
	BOOST_ASSERT(tr.size() <= 2);
	if (tr.empty())
	{
		return boost::optional<track_info_t>();
	}
	return tr.front();
}

void	TrackRecognitionHook::handle_event_end()
{
	vector< vector<wire_pos_t>* >	block;

	BOOST_FOREACH(auto &gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];
		double	max_chisq = geom.group_max_chisq[group_id];

		BOOST_FOREACH(auto &axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			const vector<chamber_id_t>	&chambers = axis_tup.second;
			bool	continue_flag = false;

			block.clear();

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block.push_back(&last_event[chamber_id]);

				if ((device_type == DEV_TYPE_DRIFT) && calibration_curve)
				{
					auto	&calib = (*calibration_curve)[chamber_id];
					if (calib.empty())
					{
						static bitset<256>	warnings_generated;
						bool	generated = warnings_generated[chamber_id];
						if (!generated)
						{
							cerr << "No calibration curve for chamber "
							     << int(chamber_id)
							     << ". Data is ignored." << endl;
						}
						warnings_generated.set(chamber_id);
						continue_flag = true;
					}
				}
			}

			if (continue_flag)
			{
				continue;
			}

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			if (device_type == DEV_TYPE_DRIFT)
			{
				//	max_chisq /= DRIFT_STEP*DRIFT_STEP;
			}
			vector<track_info_t> tracks = recognize_all_tracks<track_type_t::prop>(block, normal_pos, max_chisq);

			if ((device_type == DEV_TYPE_DRIFT) && (calibration_curve != NULL))
			{
				last_tracks[group_id][axis].clear();
				BOOST_FOREACH(track_info_t &track, tracks)
				{
					boost::optional<track_info_t> ti =
					    recognize_drift_track(track, normal_pos, chambers, max_chisq);
					if (ti)
					{
						last_tracks[group_id][axis].push_back(*ti);
					}
				}
			}
			else
			{
				last_tracks[group_id][axis] = tracks;
			}
		}
	}

	StdDrift::handle_event_end();
	last_event_finished = true;
}
