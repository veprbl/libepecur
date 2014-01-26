#ifndef __TREEEXPORTHOOK_HPP
#define __TREEEXPORTHOOK_HPP

#include <map>

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/TrackRecognitionHook.hpp>

#include <boost/unordered/unordered_map.hpp>

using boost::unordered_map;

struct event_info_t
{
	uint32_t	event_cause;
	uint32_t	timestamp;
};

struct track_group_t
{
	uint32_t	track_count;
	vector<double>	c0, c1, chisq, prev_chisq;
	vector<uint32_t>	hits_count;
	vector<double>	*c0_ptr, *c1_ptr, *chisq_ptr, *prev_chisq_ptr;
	vector<uint32_t>	*hits_count_ptr;
};

struct drift_group_t
{
	uint32_t	num_wires;
	TBranch		*wire_pos_br, *time_br;
};

struct effectivity_group_t
{
	double		effectivity;
	TBranch         *effectivity_br;
};

class TreeExportHook : public TrackRecognitionHook
{
private:

	TTree	event_tree, target_info_tree, cycle_effectivity_tree;
	event_info_t	event_info;
	unordered_map< group_id_t, map<device_axis_t, track_group_t> >	stored_track;
	unordered_map< group_id_t, map<device_axis_t, map<chamber_id_t, drift_group_t> > >	stored_drift;
	unordered_map< group_id_t, map<device_axis_t, effectivity_group_t> >	stored_effectivity;

	uint64_t	event_id;
	uint64_t	cycle_first_event_id;
	uint64_t	cycle_all_count;
	unordered_map< group_id_t, map<device_axis_t, uint64_t> >	cycle_hit_count;

	vector<char*>	names;

	void	init_track_group(
		string group_name, group_id_t group_id, device_axis_t axis
		);
	void	init_drift_group(
		string group_name, group_id_t group_id, device_axis_t axis
		);
	void	write_track_event(
		group_id_t group_id, device_axis_t axis
		);
	void	write_drift_event(
		group_id_t group_id, device_axis_t axis
		);

public:

	TreeExportHook( Geometry &g, StdDrift::calibration_curve_t *c );
	~TreeExportHook();
	const char*	store_name( string name );
	virtual void	handle_timestamp( int32_t timestamp ) override;
	virtual void	handle_trig_end_cycle() override;
	virtual void	handle_trig_info(
		uint8_t devices_mask,
		uint16_t event_cause,
		uint32_t gate_time
		) override;
	virtual void	handle_event_end();
	virtual void	handle_slow_target_info(
		const slow_target_record_t *rec
		) override;
};

#endif
