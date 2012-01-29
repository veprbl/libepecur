#ifndef __TRACK_HPP
#define __TRACK_HPP

#include "types.hpp"

#include <memory>
#include <vector>

using namespace std;

struct track_info_t
{
	double	c0;
	double	c1;
	double	chisq;
};

struct recognized_track_t
{
	track_info_t	track;
	unique_ptr<wire_pos_ptr_t[]>	wire_pos_ptr;
};

bool	next( wire_pos_ptr_t wire_pos_ptr[], const int wire_count[], const int chambers_count );
recognized_track_t	prop_recognize_track( const vector< vector<wire_pos_t>* > &data, const vector<double> &normal_pos );
vector<track_info_t>	prop_recognize_all_tracks( vector< vector<wire_pos_t>* > data, const vector<double> &normal_pos, double max_chisq );

#endif
