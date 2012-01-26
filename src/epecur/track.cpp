#include <vector>
#include <memory>

#include <gsl/gsl_fit.h>

#include "types.hpp"
#include "track.hpp"

using namespace std;

bool	next( wire_pos_ptr_t wire_pos_ptr[], const int wire_count[], const int planes_count )
/*
 * iterates over all possible wire_pos_ptr combinations
 * if next combination is not avaliable returns false
 */
{
	int	i = 0;

	while(1 + wire_pos_ptr[i] == wire_count[i])
	{
		i++;

		if (i == planes_count)
		{
			/* reached last combination */
			return false;
		}
	}

	for(int j = 0; j < i; j++)
	{
		wire_pos_ptr[j] = 0;
	}

	wire_pos_ptr[i]++;

	return true;
}

recognized_track_t	prop_recognize_track( const vector< vector<wire_pos_t> > &data )
{
	const plane_id_t	planes_count = data.size();
	unique_ptr<wire_pos_ptr_t[]>	best_wire_pos_ptr(new wire_pos_ptr_t[planes_count]);
	unique_ptr<wire_pos_ptr_t[]>	wire_pos_ptr(new wire_pos_ptr_t[planes_count]);
	unique_ptr<int[]>		wire_count(new int[planes_count]);

	// initialize variables
	{
		int plane_id = 0;

		for(auto plane_data : data)
		{
			wire_pos_ptr[plane_id] = 0;
			wire_count[plane_id] = plane_data.size();

			plane_id++;
		}
	}

	double	best_c0, best_c1;
	double	best_sumsq;
	bool	first = true;

	do
	{
		vector<double>	wires;
		vector<double>	xs;
		double	c0, c1, cov00, cov01, cov11, sumsq;
		int i = 0;

		// fill array with values to fit
		for(auto plane_data : data)
		{
			wire_pos_t	wire_id = plane_data[wire_pos_ptr[i]];

			wires.push_back(wire_id);
			xs.push_back(i); /* temporary. we should use x from geomerty */

			i++;
		}

		// perform linear fit
		gsl_fit_linear(xs.data(), 1, wires.data(), 1, wires.size(),
			       &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

		if (first || (best_sumsq > sumsq))
		{
			best_sumsq = sumsq;
			best_c0 = c0;
			best_c1 = c1;
			first = false;

			// save best wires array positions
			for(plane_id_t plane_id = 0; plane_id < planes_count; plane_id++)
			{
				best_wire_pos_ptr[plane_id] = wire_pos_ptr[plane_id];
			}
		}
	}
	while(next( wire_pos_ptr.get(), &wire_count[0], planes_count ));

	return recognized_track_t{
		track_info_t{best_c0, best_c1},
		move(best_wire_pos_ptr)
		};
}

vector<track_info_t>	prop_recognize_all_tracks( vector< vector<wire_pos_t> > data )
{
	vector<track_info_t>	result;
	bool	next_track_clearance = true;

	// check if there is any data
	if (data.size() == 0)
	{
			// return no tracks
			return vector<track_info_t>();
	}

	// check if there is empty plane
	for(auto &plane_data : data)
	{
		if (plane_data.empty())
		{
			// return no tracks
			return vector<track_info_t>();
		}
	}

	do
	{
		recognized_track_t	info = prop_recognize_track(data);
		track_info_t	&track = info.track;

		result.push_back(track);

		plane_id_t	plane_id = 0;

		for(auto &plane_data : data)
		{
			plane_data.erase(plane_data.begin() + info.wire_pos_ptr[plane_id]);

			if (plane_data.empty())
			{
				next_track_clearance = false;
				break;
			}

			plane_id++;
		}
	}
	while(next_track_clearance);

	return result;
}
