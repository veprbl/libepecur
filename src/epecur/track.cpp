#include <vector>
#include <memory>

#include <gsl/gsl_fit.h>

#include "types.hpp"
#include "track.hpp"

using namespace std;

bool	next( wire_pos_ptr_t wire_pos_ptr[], const int wire_count[], const int chambers_count )
/*
 * iterates over all possible wire_pos_ptr combinations
 * if next combination is not avaliable returns false
 */
{
	int	i = 0;

	while(1 + wire_pos_ptr[i] == wire_count[i])
	{
		i++;

		if (i == chambers_count)
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

bool	delete_empty_chambers( vector< vector<wire_pos_t>* > &data, vector<double> &normal_pos, vector<uint> &used_chambers )
/*
 * deletes every i-th record if data[i]->empty()
 * returns false if there is no enough non-empty chambers left
 */
{
	uint	empty_chambers_count = 0;
	auto	data_it = data.begin();
	auto	normal_pos_it = normal_pos.begin();
	auto	used_chambers_it = used_chambers.begin();

	// check if there is enough non-empty chambers
	while(data_it != data.end())
	{
		if ((*data_it)->empty())
		{
			empty_chambers_count++;

			if (data.size() <= MIN_TRACK_CHAMBERS)
			{
				return false;
			}

			data_it = data.erase(data_it);
			normal_pos_it = normal_pos.erase(normal_pos_it);
			used_chambers_it = used_chambers.erase(used_chambers_it);
		}
		else
		{
			data_it++;
			normal_pos_it++;
			used_chambers_it++;
		}
	}

	return true;
}

recognized_track_t	prop_recognize_track( const vector< vector<wire_pos_t>* > &data, const vector<double> &normal_pos )
{
	const chamber_id_t	chambers_count = data.size();
	unique_ptr<wire_pos_ptr_t[]>	best_wire_pos_ptr(new wire_pos_ptr_t[chambers_count]);
	unique_ptr<wire_pos_ptr_t[]>	wire_pos_ptr(new wire_pos_ptr_t[chambers_count]);
	unique_ptr<int[]>		wire_count(new int[chambers_count]);

	// initialize variables
	{
		int chamber_id = 0;

		for(auto chamber_data : data)
		{
			wire_pos_ptr[chamber_id] = 0;
			wire_count[chamber_id] = chamber_data->size();

			chamber_id++;
		}
	}

	double	best_c0, best_c1;
	double	best_sumsq;
	vector<double>	best_wires_pos;
	bool	first = true;

	do
	{
		vector<double>	wires;
		double	c0, c1, cov00, cov01, cov11, sumsq;
		int i = 0;

		// fill array with values to fit
		for(auto chamber_data : data)
		{
			wire_pos_t	wire_id = (*chamber_data)[wire_pos_ptr[i]];

			wires.push_back(wire_id);

			i++;
		}

		// perform linear fit
		gsl_fit_linear(normal_pos.data(), 1, wires.data(), 1, wires.size(),
			       &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

		if (first || (best_sumsq > sumsq))
		{
			best_sumsq = sumsq;
			best_c0 = c0;
			best_c1 = c1;
			best_wires_pos = wires;
			first = false;

			// save best wires array positions
			for(chamber_id_t chamber_id = 0; chamber_id < chambers_count; chamber_id++)
			{
				best_wire_pos_ptr[chamber_id] = wire_pos_ptr[chamber_id];
			}
		}
	}
	while(next( wire_pos_ptr.get(), &wire_count[0], chambers_count ));

	return recognized_track_t{
		track_info_t{best_c0, best_c1, best_sumsq, best_wires_pos},
		move(best_wire_pos_ptr)
		};
}

vector<track_info_t>	prop_recognize_all_tracks( vector< vector<wire_pos_t>* > data, vector<double> normal_pos, double max_chisq )
/*
 * Warning: This function will delete recognized wires from your original vectors.
 */
{
	vector<uint>	used_chambers;
	vector<track_info_t>	result;

	for(uint i = 0; i < data.size(); i++)
	{
		used_chambers.push_back(i);
	}

	// check if there is enough chambers
	if (data.size() < MIN_TRACK_CHAMBERS)
	{
		throw "Not enough chambers in original data!";
	}

	while(delete_empty_chambers(data, normal_pos, used_chambers))
	{
		recognized_track_t	info = prop_recognize_track(data, normal_pos);
		track_info_t	&track = info.track;

		track.used_chambers = used_chambers;

		if ((max_chisq > 0) && (info.track.chisq > max_chisq))
		{
			break;
		}

		result.push_back(track);

		chamber_id_t	chamber_id = 0;

		for(auto chamber_data : data)
		{
			chamber_data->erase(chamber_data->begin() + info.wire_pos_ptr[chamber_id]);

			chamber_id++;
		}
	}

	return result;
}
