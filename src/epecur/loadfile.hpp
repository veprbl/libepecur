#ifndef __LOADFILE_HPP
#define __LOADFILE_HPP

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

	virtual void	handle_prop_data( const char* begin, const char* end, uint16_t dev_id, void* data ) {};
	virtual void	handle_drift_data( char wire_id, uint16_t time ) {};
	virtual void	handle_event_end() {};
};

void	loadfile( std::string filename, LoadHook &hook );

#endif
