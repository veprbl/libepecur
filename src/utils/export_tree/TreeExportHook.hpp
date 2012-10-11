#ifndef __TREEEXPORTHOOK_HPP
#define __TREEEXPORTHOOK_HPP

#include <map>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/TrackRecognitionHook.hpp>

#include <boost/unordered/unordered_map.hpp>

using boost::unordered_map;

struct prop_group_t
{
	uint32_t	track_count;
	vector<double>	c0, c1, chisq;
	vector<uint32_t>	hits_count;
	TBranch		*c0_br, *c1_br, *chisq_br, *hits_count_br;
};

struct drift_group_t
{
	uint32_t	num_wires;
	TBranch		*wire_pos_br, *time_br;
};

class TreeExportHook : public TrackRecognitionHook
{
private:

	TTree	event_tree;
	unordered_map< group_id_t, map<device_axis_t, prop_group_t> >	stored_prop;
	unordered_map< group_id_t, map<device_axis_t, map<int, drift_group_t> > >	stored_drift;

	vector<char*>	names;

	void	init_prop_group(
		string group_name, group_id_t group_id, device_axis_t axis
		);
	void	init_drift_group(
		string group_name, group_id_t group_id, device_axis_t axis
		);
	void	write_prop_event(
		group_id_t group_id, device_axis_t axis
		);
	void	write_drift_event(
		group_id_t group_id, device_axis_t axis
		);

public:

	TreeExportHook( Geometry &g, double max_chisq = -1 );
	~TreeExportHook();
	const char*	store_name( string name );
	virtual void	handle_event_end();
};

#endif
