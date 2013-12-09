#ifndef __LOADFILE_HPP
#define __LOADFILE_HPP

#include <vector>

#include "types.hpp"

enum device_type_t
{
	DEV_TYPE_PROP	= 0,
	DEV_TYPE_DRIFT	= 1,
	DEV_TYPE_HODO	= 2,
	DEV_TYPE_TRIG	= 3,
	DEV_TYPE_CAMAC	= 4
};

enum record_type_t
{
	REC_TYPE_CYCLE	= 0x100,
	REC_TYPE_SLOW	= 0x1000,
	REC_TYPE_RAW	= 0x10000
};

enum slow_type_t
{
	EP_SRCID_TGT	= 1001,
	EP_SRCID_NMR	= 1002,
	EP_SRCID_TRGB  	= 1003,
	EP_SRCID_CAMERA	= 1004
};

typedef struct
{
	double	R[8];
	double	V[7];		// pressure sensors V[0-3] and vacuum sensors V[4-7]
	double	H[8];
	int32_t	manual;	// R[manual] is invalid
	int32_t	time;	// measurement UNIX timestamp
} slow_target_record_t;

class	LoadHook
{
public:

	virtual ~LoadHook() {};
	virtual void	handle_timestamp( int32_t timestamp ) {};
	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id ) {};
	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) {};
	virtual void	handle_trig_end_cycle() {};
	virtual void	handle_trig_info(
		uint8_t devices_mask,
		uint16_t event_cause,
		uint32_t gate_time
		) {};
	virtual void	handle_event_end() {};
	virtual void	handle_slow_target_info(
		const slow_target_record_t *rec
		) {};
};

void	loadfile( std::string filename, LoadHook &hook );

#endif
