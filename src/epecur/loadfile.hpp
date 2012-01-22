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

struct load_hooks_t
{
	void	(*prop_handler)( const char* begin, const char* end, uint16_t dev_id, void* data );
	void	(*drift_handler)( char wire_id, uint16_t time );
};

void	loadfile( std::string filename, load_hooks_t &hooks );

#endif
