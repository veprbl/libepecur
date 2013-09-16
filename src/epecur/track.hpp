#ifndef __TRACK_HPP
#define __TRACK_HPP

#include "types.hpp"

#include <vector>

#include <boost/detail/scoped_enum_emulation.hpp>

using namespace std;

struct track_info_t
{
	double	c0;
	double	c1;
	double	chisq;
	double	prev_chisq;
	vector<double>	chamber_wires_pos;
	vector<wire_pos_ptr_t>	wire_pos_ptr;
	vector<uint>	used_chambers;
};

BOOST_SCOPED_ENUM_START(track_type)
{ prop, drift };
BOOST_SCOPED_ENUM_END
typedef BOOST_SCOPED_ENUM(track_type) track_type_t;

const uint	MIN_TRACK_CHAMBERS = 3;

bool	next( vector<wire_pos_ptr_t> &wire_pos_ptr, const int wire_count[], const int chambers_count );
track_info_t	recognize_track( const vector< vector<wire_pos_t>* > &data, const vector<double> &normal_pos );
template<track_type_t track_type>
vector<track_info_t>	recognize_all_tracks( vector< vector<wire_pos_t>* > data, vector<double> normal_pos, double max_chisq = -1 );

#endif
