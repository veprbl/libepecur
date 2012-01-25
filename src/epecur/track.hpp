#ifndef __TRACK_HPP
#define __TRACK_HPP

#include "types.hpp"

#include <vector>

using namespace std;

struct track_info_t
{
	double	c0;
	double	c1;
};

bool	next( wire_pos_ptr_t wire_pos_ptr[], const int wire_count[], const int planes_count );
track_info_t	prop_recognize_track( const vector< vector<wire_pos_t> > &data );

#endif
