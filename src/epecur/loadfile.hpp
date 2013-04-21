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

class	LoadHook
{
public:

	virtual ~LoadHook() {};
	virtual LoadHook*	get_copy() { return NULL; };
	virtual void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id ) {};
	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) {};
	virtual void	handle_event_end() {};
};

void	loadfile( std::string filename, LoadHook &hook );

#endif
