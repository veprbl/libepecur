#include <cstdlib>
#include <cmath>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "TrackReconstructionHook.hpp"

TrackReconstructionHook::TrackReconstructionHook(
	Geometry &g,
	StdHits::calibration_curve_t *c
	)
	: StdHits(g, c),
	  geom(g)
{
	// nothing
}

void	TrackReconstructionHook::handle_event_start()
{
	StdHits::handle_event_start();

	BOOST_FOREACH(auto gr_tup, last_tracks)
	{
		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			axis_tup.second.clear();
		}
	}
}

void	TrackReconstructionHook::handle_event_end()
{
	StdHits::handle_event_end();

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

			block.clear();

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				block.push_back(&last_event[chamber_id]);
			}

			vector<double>	&normal_pos = geom.normal_pos[group_id][axis];

			if ((device_type == DEV_TYPE_PROP) ||
			    ((device_type == DEV_TYPE_DRIFT) && (calibration_curve == NULL)))
			{
				last_tracks[group_id][axis] = reconstruct_all_tracks<track_type_t::prop>(block, normal_pos, max_chisq);
			}
			else if (device_type == DEV_TYPE_DRIFT)
			{
				last_tracks[group_id][axis] = reconstruct_all_tracks<track_type_t::drift>(block, normal_pos, max_chisq);
			}
			else
			{
				throw "Invalid dev type";
			}
		}
	}
}
