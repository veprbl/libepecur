#ifndef __TREEEXPORTHOOK_HPP
#define __TREEEXPORTHOOK_HPP

#include <map>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/TrackRecognitionHook.hpp>

#include <boost/unordered/unordered_map.hpp>

using boost::unordered_map;

struct stored_group_t
{
	uint32_t	track_count;
	vector<double>	c0, c1, chisq;
	vector<uint32_t>	hits_count;
	TBranch		*c0_br, *c1_br, *chisq_br, *hits_count_br;
};

struct stored_drift_times_t
{
	chamber_id_t	chamber_id;
	uint32_t	num_wires;
	vector<wire_pos_t>	wire_pos;
	vector<uint16_t>	time;
	TBranch		*wire_pos_br, *time_br;
};

class TreeExportHook : public TrackRecognitionHook
{
private:

	TTree	tree;
	unordered_map< group_id_t, map<device_axis_t, stored_group_t> >	stored_group;

	TTree	drift_tree;
	stored_drift_times_t	stored_drift;

	vector<char*>	names;

public:

	TreeExportHook( Geometry &g, double max_chisq = -1 );
	~TreeExportHook();
	const char*	store_name( string name );
	virtual void	handle_event_end();
};

#endif
