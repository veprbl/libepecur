#include <cstdlib>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "TrackRecognitionHook.hpp"

TrackRecognitionHook::TrackRecognitionHook( Geometry &g, double max_chisq )
	: last_event_finished(false),
	  drift_cleanup(false),
	  max_chisq(max_chisq),
	  geom(g)
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

	for(auto pos = begin; pos < end; pos++)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, *pos);

		chamber.push_back(wire_pos);
	}
}

void	TrackRecognitionHook::handle_drift_data(
	std::vector<wire_id_t> &wire_id_s,
	std::vector<uint16_t> &time_s,
	device_id_t dev_id
	)
{
	if (drift_cleanup)
	{
		last_event_drift_wire_pos.clear();
		last_event_drift_time.clear();

		drift_cleanup = false;
	}

	chamber_id_t	chamber_id = geom.get_device_chamber(dev_id);
	auto	&event_wire_pos = last_event_drift_wire_pos[chamber_id];
	auto	&event_time = last_event_drift_time[chamber_id];

	BOOST_ASSERT(wire_id_s.size() == time_s.size());

	auto	time_it = time_s.begin();
	wire_pos_t	prev_wire_pos = 0;
	uint16_t	prev_time = 0;

	if (event_wire_pos.size() > 0)
	{
		prev_wire_pos = event_wire_pos.back();
		prev_time = event_time.back();
	}

	BOOST_FOREACH(wire_id_t wire_id, wire_id_s)
	{
		wire_pos_t	wire_pos = geom.get_wire_pos(dev_id, wire_id);
		uint16_t	time = (*time_it);
		bool		append = true;

		if ((event_wire_pos.size() > 0)
		    && (abs(prev_wire_pos - wire_pos) == 2))
		{
			int16_t	timedelta = time - prev_time;

			if ((timedelta >= -4) && (timedelta < 0))
			{
				event_wire_pos.back() = wire_pos;
				event_time.back() = time;
				prev_wire_pos = wire_pos;
				prev_time = time;

				append = false;
			}

			if ((timedelta > 0) && (timedelta <= 4))
			{
				append = false;
			}
		}

		if (append)
		{
			event_wire_pos.push_back(wire_pos);
			event_time.push_back(time);
			prev_wire_pos = wire_pos;
			prev_time = time;
		}

		time_it++;
	}

	BOOST_ASSERT(event_wire_pos.size() == event_time.size());
}

void	TrackRecognitionHook::handle_event_end()
{
	vector< vector<wire_pos_t>* >	block;

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		if (geom.group_device_type[group_id] != DEV_TYPE_PROP)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	chambers = axis_tup.second;

			block.clear();

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block.push_back(&last_event[chamber_id]);
			}

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			last_tracks[group_id][axis] = prop_recognize_all_tracks(block, normal_pos, max_chisq);
		}
	}

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type != DEV_TYPE_DRIFT)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	&chambers =
				geom.group_chambers[group_id][axis];

			if (chambers.size() < MIN_TRACK_CHAMBERS)
			{
				continue;
			}

			block.clear();
			vector< vector<wire_pos_t> >	block_copy;

			block_copy.reserve(chambers.size());

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block_copy.push_back(last_event_drift_wire_pos[chamber_id]);
				block.push_back(&block_copy.back());
			}

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			last_tracks[group_id][axis] = prop_recognize_all_tracks(block, normal_pos, max_chisq);

			BOOST_FOREACH(track_info_t &track, last_tracks[group_id][axis])
			{
				auto	chamber_index_it = track.used_chambers.begin();
				auto	wire_index_it = track.wire_pos_ptr.begin();

				double	DRIFT_STEP = 17.0; // mm
				double	psi = atan(track.c1 * DRIFT_STEP);

				psi = abs(psi);

				BOOST_ASSERT(psi < M_PI/2);

				if (psi > M_PI/3)
				{
					psi -= M_PI/3;
				}

				/*
				  sin(60*)   sin(120*-psi)        l      sin(60*)
				  -------- = -------------  <=>  --- = -------------
				      l           l0              l0   sin(120*-psi)
				 */

				double	multiplier = (sqrt(3)/2) / (sin(2*M_PI/3 - psi));

				BOOST_ASSERT(multiplier >= 0);
				BOOST_ASSERT(multiplier <= 1);

				BOOST_FOREACH(wire_pos_t pos, track.chamber_wires_pos)
				{
					chamber_id_t	chamber_id = chambers[*(chamber_index_it++)];

					vector<uint16_t>	&time =
						last_event_drift_time[chamber_id];

					uint16_t	t = time[*(wire_index_it++)];

					auto	&calib = time_distributions[chamber_id];

					if (calib.find(pos) == calib.end())
					{
						calib[pos] =
							vector<unsigned int>(MAX_TIME_COUNTS);
					}

					uint16_t	calib_t = round(t / multiplier);

					if (calib_t < MAX_TIME_COUNTS)
					{
						calib[pos][calib_t]++;
					}
				}
			}
		}
	}

	last_event_finished = true;
	drift_cleanup = true;
}
