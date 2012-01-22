#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <cstddef>

#include <boost/assert.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/endian/integers.hpp>

#include "loadfile.hpp"

using namespace boost::endian;
using namespace std;

#pragma pack(push, 1)

typedef struct
{
	ulittle32_t	length;		// full record length in bytes
	ulittle32_t	id;		// id of event in file
	ulittle32_t	flags;		// bit flags
} cycle_event_header_t;

typedef struct
{
	ulittle32_t length;	// full record length in bytes
	ulittle32_t type;	// record type
	ulittle32_t time;	// record creation time UNIX timestamp

	ulittle32_t number;	// record id or id of cycle
	ulittle32_t flags;
} record_header_t;

#pragma pack(pop)

const uint	END_OF_CYCLE_FLAG(0x80000000);

const char*	file_start;

template< class T >
T	read_magic_integer( const char* &pos, uint bytes )
{
	char	c;
	T	result = 0;

	BOOST_ASSERT(sizeof(T) * 8 >= bytes * 6);

	for(uint i = 0; i < bytes; i++)
	{
		c = *(pos++);

		if(c & 0xC0)
		{
			cerr << int(pos) << ", *(pos-1) = " << int(c) << endl;
			throw "Non-zero 7-8th bits in magic integer";
		}

		result += c << (i * 6);
	}

	return result;
}

/*
template< class T >
inline void	mem_read( const char* &pos, T *dest )
{
	T	result;

	memcpy(reinterpret_cast<char*>(dest), pos, sizeof(*dest));
	pos += sizeof(*dest);
}
*/

 /*
   Evil version of the code above. Saves few precious seconds.
  */
#define	mem_read(pos, dest)				   \
	memcpy(reinterpret_cast<char*>(&dest), pos, sizeof(dest)); \
	pos += sizeof(dest);


void	skip_magic_data( const char* &pos, const char* max_pos )
{
	char	c;

	while(pos < max_pos)
	{
		c = *(pos++);

		if (c & 0x80)
		{
			pos--;
			break;
		}
	}
}

void	read_prop_data(
	const char* &pos, const char* max_pos,
	int	dev_id,
	load_hooks_t &hooks
	)
{
	auto	orig_pos = pos;

	skip_magic_data(pos, max_pos);

	if (hooks.prop_handler)
	{
		hooks.prop_handler(orig_pos, pos, dev_id, nullptr);
	}
}

void	read_drift_data(
	const char* &pos, const char* max_pos,
	int	dev_id,
	load_hooks_t &hooks
	)
{
	while(pos < max_pos)
	{
		char	wire_id;
		uint16_t	time;

		wire_id	= *(pos++);

		if (wire_id & 0x80)
		{
			pos--;

			if (pos > max_pos)
			{
				throw "Broke out while reading drift data!";
			}

			break;
		}

		if (!(wire_id & 0x40))
		{
			throw "Expected non-zero 7-th bit.";
		}

		time	= read_magic_integer<decltype(time)>(pos, 2);
	}
}

void	handle_trig_end_cycle( const char* &pos, const char* max_pos )
{
	typedef	uint32_t counter_value_t;

	counter_value_t	LAST_COUNTER_ID = 0x7F4F;
	map<uint16_t, counter_value_t>	counter_values;

	auto	cycle_id = read_magic_integer<int16_t>(pos, 2);

	pos += 3;

	while(1)
	{
		if (pos > max_pos)
		{
			throw "Expected ending counter, found end of stream";
		}

		ulittle16_t	counter_id;

		mem_read(pos, counter_id);

		counter_values[counter_id] = read_magic_integer<counter_value_t>(pos, 4);

		if (counter_id == LAST_COUNTER_ID)
		{
			break;
		}
	}

	if (counter_values.count(0x404E) && counter_values.count(0x414E))
	{
		auto	freq = float(counter_values[0x404E]) / counter_values[0x414E] / 10;
	}
	else
	{
		cerr << "no freq" << endl;
	}

	skip_magic_data(pos, max_pos);
}

void	read_cycle_stream( const char* &pos, const char* max_pos, int32_t flags, load_hooks_t &hooks )
{
	ubig16_t	stream_header;
	uint16_t	dev_id;
	device_type_t	dev_type;

	while(pos < max_pos)
	{
		mem_read(pos, stream_header);

		if (!(stream_header & 0x8000))
		{
			throw "Wrong file format!";
		}

		dev_type =
			static_cast<device_type_t>(
				(stream_header & 0x7000) >> 12
				);
		dev_id = stream_header & 0x0FFF;

		switch(dev_type)
		{
		case DEV_TYPE_PROP:

			read_prop_data(pos, max_pos, dev_id, hooks);

			break;
		case DEV_TYPE_DRIFT:

			read_drift_data(pos, max_pos, dev_id, hooks);

			break;
		case DEV_TYPE_TRIG:
			if (flags & END_OF_CYCLE_FLAG)
			{
				handle_trig_end_cycle(pos, max_pos);
				break;
			}
		case DEV_TYPE_HODO:
		case DEV_TYPE_CAMAC:
		default:

			skip_magic_data(pos, max_pos);

			break;
		}
	}
}

void	read_cycle( const char* &pos, const char *max_pos, load_hooks_t &hooks )
{
	cycle_event_header_t	ev;

	while(pos < max_pos)
	{
		mem_read(pos, ev);

		read_cycle_stream(pos, pos + ev.length - sizeof(ev), ev.flags, hooks);
	}
}

bool	is_valid_record( record_header_t &rec )
{
	if ((rec.type != REC_TYPE_CYCLE)
	    & (rec.type != REC_TYPE_SLOW)
	    & (rec.type != REC_TYPE_RAW))
	{
		return false;
	}

/*	if (rec.length > 5*1024*1024) // 5 mb
	{
		return false;
	}*/

	if ((rec.time < 1000000000) // Sun Sep  9 05:46:40 2001
	    | (rec.time > 2000000000)) // Wed May 18 07:33:20 2033
	{
		return false;
	}

	return true;
}

void	resync( const char* &pos, const char* file_end )
{
	bool	is_done = false;
	record_header_t	next_rec;
	const char*	next_rec_pos;

	do
	{
		while(*(reinterpret_cast<const ulittle32_t*>(pos)) != END_OF_CYCLE_FLAG)
		{
			pos++;

			if (pos >= file_end)
			{
				throw "Reached end of file while resyncing.";
			}
		}

		auto	event_pos = pos - offsetof(cycle_event_header_t, flags);
		auto	&event = *reinterpret_cast<const cycle_event_header_t*>(event_pos);

		pos++; // Increment pos now, so we move forward on next iteration.

		if (event.length > 5000 || event.length == 0)
		{
			continue;
		}

		next_rec_pos = event_pos + event.length;

		if (next_rec_pos >= file_end)
		{
			continue;
		}

		mem_read(next_rec_pos, next_rec);

/*		cerr << "Found record with following attributes:" << endl;
		cout << "length: " << next_rec.length << endl;
		cout << "type:   0x" << hex << next_rec.type << endl;
		cout << "flags:  0x" << hex << next_rec.flags << endl;*/

		is_done = is_valid_record(next_rec);
	}
	while(!is_done);

	pos = next_rec_pos - sizeof(next_rec);
}

void	loadfile( string filename, load_hooks_t &hooks )
{

	boost::iostreams::mapped_file_source m_file_(filename.c_str());

	auto	size = m_file_.size();

	if (!m_file_.is_open())
	{
		throw "Couldn't open data file!";
	}

	const char*	pos = m_file_.data();
	const char*	file_end = pos + size;

	file_start = pos;

	cout << "beginning = " << int(pos) << endl;
	cout << "size = " << size / 1024 / 1024 << " mb" << endl;

	while(pos < file_end)
	{
		record_header_t	rec;

		mem_read(pos, rec);

		const char *record_end = pos + rec.length - sizeof(rec);

		if (record_end > file_end)
		{
			throw "Unfinished record!";
		}

		if (record_end + sizeof(record_header_t) < file_end)
		{
			auto		next_rec_pos = record_end;
			record_header_t	next_rec;

			mem_read(next_rec_pos, next_rec);

			if (!is_valid_record(next_rec))
			{
				cerr << "Record seems to be bad. Trying to resync...";

				try
				{
					resync(pos, file_end);
				}
				catch(const char* msg)
				{
					cerr << endl << msg << endl;
					return;
				}

				cerr << " success" << endl;;
				continue;
			}
		}

		switch(rec.type)
		{
		case REC_TYPE_CYCLE:

			read_cycle(pos, record_end, hooks);

			break;
		case REC_TYPE_SLOW:
		case REC_TYPE_RAW:

			pos = record_end;

			break;
		default:

			cerr << rec.type << endl;
			cerr << "unkonwn block addr " << int(pos) << endl;
			pos = record_end;
			throw "Unknown record type!";
		}

		if (pos != record_end)
		{
			throw "Block read function didn't stopped at the end of a block";
		}
	}
}
