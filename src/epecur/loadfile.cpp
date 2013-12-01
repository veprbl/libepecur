#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <cstddef>

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/endian/integers.hpp>

#include "types.hpp"
#include "loadfile.hpp"

using namespace boost::endian;
using namespace std;

#pragma pack(push, 1)

typedef struct
{
	ulittle32_t	length;		// full record length in bytes
	ulittle32_t	id;		// id of event in file
	ulittle32_t	flags;		// bit flags
} event_header_t;

typedef struct
{
	ulittle32_t	length;
	ulittle32_t	type;
} slow_header_t;

typedef struct
{
	ulittle32_t length;	// full record length in bytes
	ulittle32_t type;	// record type
	ulittle32_t time;	// record creation time UNIX timestamp

	ulittle32_t number;	// record id or id of cycle
	ulittle32_t flags;
} record_header_t;

#pragma pack(pop)

BOOST_STATIC_ASSERT(sizeof(event_header_t) == 12);
BOOST_STATIC_ASSERT(sizeof(slow_target_record_t) == 192);
BOOST_STATIC_ASSERT(sizeof(record_header_t) == 20);

const uint	END_OF_CYCLE_FLAG(0x80000000);

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
	device_id_t	dev_id,
	LoadHook &hook
	)
{
	auto	orig_pos = pos;

	skip_magic_data(pos, max_pos);

	hook.handle_prop_data(
		reinterpret_cast<const wire_id_t*>(orig_pos),
		reinterpret_cast<const wire_id_t*>(pos),
		dev_id
		);
}

void	read_drift_data(
	const char* &pos, const char* max_pos,
	device_id_t	dev_id,
	LoadHook &hook
	)
{
	std::vector<wire_id_t>	result_wire_id;
	std::vector<uint16_t>	result_time;

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

		wire_id	= wire_id & 0x3F;
		time	= read_magic_integer<decltype(time)>(pos, 2);

		result_wire_id.push_back(wire_id);
		result_time.push_back(time);
	}

	BOOST_ASSERT(result_wire_id.size() == result_time.size());

	hook.handle_drift_data(result_wire_id, result_time, dev_id);
}

void	handle_trig_normal(
	const char* &pos, const char* max_pos,
	LoadHook &hook
	)
{
	uint8_t	    devices_mask;
	uint16_t    event_cause;
	uint32_t    gate_time;

	devices_mask =
	    read_magic_integer<decltype(devices_mask)>(pos, 1);
	event_cause =
	    read_magic_integer<decltype(event_cause)>(pos, 2);
	gate_time =
	    read_magic_integer<decltype(gate_time)>(pos, 3);

	hook.handle_trig_info(devices_mask, event_cause, gate_time);
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

void	read_event( const char* &pos, const char* max_pos, int32_t flags, LoadHook &hook )
{
	ubig16_t	stream_header;
	device_id_t	dev_id;
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
		dev_id = ((stream_header & 0x0F00) >> 1) | (stream_header & 0x007F);

		switch(dev_type)
		{
		case DEV_TYPE_PROP:

			read_prop_data(pos, max_pos, dev_id, hook);

			break;
		case DEV_TYPE_DRIFT:

			read_drift_data(pos, max_pos, dev_id, hook);

			break;
		case DEV_TYPE_TRIG:
			if (flags & END_OF_CYCLE_FLAG)
			{
				handle_trig_end_cycle(pos, max_pos);
			}
			else
			{
				handle_trig_normal(pos, max_pos, hook);
			}
			break;
		case DEV_TYPE_HODO:
		case DEV_TYPE_CAMAC:
		default:

			skip_magic_data(pos, max_pos);

			break;
		}
	}

	hook.handle_event_end();
}

void	read_cycle( const char* &pos, const char *max_pos, LoadHook &hook )
{
	event_header_t	event;

	while(pos < max_pos)
	{
		mem_read(pos, event);

		read_event(pos, pos + event.length - sizeof(event), event.flags, hook);
	}
}

void	read_slow( const char* &pos, const char *max_pos, LoadHook &hook )
{
	slow_header_t	slow_header;
	const char	*record_max_pos;

	while(pos < max_pos)
	{
		mem_read(pos, slow_header);
		record_max_pos = pos + slow_header.length - sizeof(slow_header);

		switch(slow_header.type)
		{
		case EP_SRCID_TGT:

			BOOST_ASSERT(slow_header.length == 200);
			const slow_target_record_t	*rec;
			rec = reinterpret_cast<decltype(rec)>(pos);
			hook.handle_slow_target_info(rec);
			pos = record_max_pos;

			break;
		case EP_SRCID_NMR:
		case EP_SRCID_TRGB:
		case EP_SRCID_CAMERA:
		default:

			pos = record_max_pos;

			break;
		}
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

void	resync( const char* &pos, const char* window_end )
{
	bool	is_done = false;
	record_header_t	next_rec;
	const char*	next_rec_pos;

	do
	{
		while(*(reinterpret_cast<const ulittle32_t*>(pos)) != END_OF_CYCLE_FLAG)
		{
			pos++;

			if (pos >= window_end - sizeof(ulittle32_t))
			{
				throw "Reached end of window while resyncing.";
			}
		}

		auto	event_pos = pos - offsetof(event_header_t, flags);
		auto	&event = *reinterpret_cast<const event_header_t*>(event_pos);

		pos++; // Increment pos now, so we move forward on next iteration.

		if (event.length > 5000 || event.length == 0)
		{
			continue;
		}

		next_rec_pos = event_pos + event.length;

		if (next_rec_pos >= window_end)
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

bool	read_record( const char* &pos, const char* window_end, bool is_last_window, LoadHook &hook )
{
	record_header_t	rec;

	mem_read(pos, rec);

	const char *record_end = pos + rec.length - sizeof(rec);

	if (record_end > window_end)
	{
		if (is_last_window)
		{
			throw "Unfinished record!";
		}
		else
		{
			return false;
		}
	}

	if (record_end + sizeof(record_header_t) < window_end)
	{
		auto		next_rec_pos = record_end;
		record_header_t	next_rec;

		mem_read(next_rec_pos, next_rec);

		if (!is_valid_record(next_rec))
		{
			cerr << "Record seems to be bad. Trying to resync...";

			try
			{
				resync(pos, window_end);
			}
			catch(const char* msg)
			{
				cerr << endl << msg << endl;
				return false;
			}

			cerr << " success" << endl;;
			return true;
		}
	}
	else
	{
		if (!is_last_window)
		{
			// this should never happen really
			return false;
		}
	}

	switch(rec.type)
	{
	case REC_TYPE_CYCLE:

		try
		{
			read_cycle(pos, record_end, hook);
		}
		catch (char const* e)
		{
			cerr << e << endl;
			pos = record_end;
		}

		break;
	case REC_TYPE_SLOW:

		read_slow(pos, record_end, hook);

		break;
	case REC_TYPE_RAW:

		pos = record_end;

		break;
	default:

		cerr << rec.type << endl;
		cerr << "unknown block addr " << hex << pos << endl;
		pos = record_end;
		throw "Unknown record type!";
	}

	if (pos != record_end)
	{
		throw "Block read function didn't stopped at the end of a block";
	}

	return true;
}

const int64_t	MAX_WINDOW_SIZE = 1024*1024*1024; // 1 gigabyte

using boost::uintmax_t;
using boost::iostreams::mapped_file_source;

mapped_file_source*	new_window( string filename, uintmax_t file_size, uintmax_t window_pos, uintmax_t &window_size, bool &is_last_window )
{
	mapped_file_source	*m_file_;

	// Determine new window size
	window_size = MAX_WINDOW_SIZE;

	// Check if this window is last
	is_last_window = window_pos + window_size >= file_size;

	// and if it is, truncate its size to fit into file
	if (is_last_window)
	{
		window_size = file_size - window_pos;
	}

	m_file_ = new mapped_file_source(
		filename,
		window_size,
		window_pos
		);

	if (!m_file_->is_open())
	{
		throw "Couldn't open data file!";
	}

	if (m_file_->size() != window_size)
	{
		throw "Wrong mapped region size";
	}

	return m_file_;
}

void	loadfile( string filename, LoadHook &hook )
{
	mapped_file_source	*m_file_;

	uintmax_t	size = boost::filesystem::file_size(filename);
	uintmax_t	window_pos = 0;
	uintmax_t	window_size;
	uintmax_t	last_fail_pos = -1;

	bool	is_last_window;

	m_file_ = new_window(filename, size, window_pos, window_size, is_last_window);

	// Reading cursor current position in memory
	const char*	pos = m_file_->data();
	// Position of window end in memory
	const char*	window_end = pos + window_size;

	cout << "size = " << size / 1024 / 1024 << " mb" << endl;

	do
	{
		auto	backup = pos;
		bool	result = read_record(pos, window_end, is_last_window, hook);

		if (!result)
		{
			// If reading operation failed, restore reading cursor
			pos = backup;

			// Determine position in window
			uintmax_t	relative_pos = pos - (window_end - window_size);
			// Determine absolute pos in file
			uintmax_t	absolute_pos = window_pos + relative_pos;

			if (last_fail_pos == absolute_pos)
			{
				// If we see read_record to fail two times in a
				// row in one place of file we just throw an
				// exception
				throw "Record data doesn't fit in window";
			}

			last_fail_pos = absolute_pos;

			// Position of mmaped region must be aligned
			auto	shift = absolute_pos % mapped_file_source::alignment();
			window_pos = absolute_pos - shift;

			delete m_file_;

			m_file_ = new_window(filename, size, window_pos, window_size, is_last_window);

			window_end	= m_file_->data() + window_size;
			pos		= m_file_->data() + shift;
		}
	}
	while(!(is_last_window && (pos >= window_end)));

	delete m_file_;
}
