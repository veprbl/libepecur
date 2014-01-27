#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <epecur/loadfile.hpp>

BOOST_AUTO_TEST_SUITE( loadfile_test )

const unsigned char	prop_data[] =
{
	0x4E, 0x00, 0x00, 0x00, // record length
	0x00, 0x01, 0x00, 0x00, // type = cycle
	0x8C, 0xCD, 0x2C, 0x4C, // Thu Jul  1 21:17:00 2010
	0x87, 0x65, 0x43, 0x21, // record id
	0x00, 0x00, 0x00, 0x00, // flags

      	0x0F, 0x00, 0x00, 0x00, // event length
      	0x12, 0x34, 0x56, 0x78, // event id
      	0x00, 0x00, 0x00, 0x00, // event flag

	0x80, 0x14, // prop, id = 20
	0x2A, // wire 0x2A (42)

      	0x2B, 0x00, 0x00, 0x00, // event length
      	0x12, 0x34, 0x56, 0x78, // event id
      	0x00, 0x00, 0x00, 0x80, // event flag (EOC)

	0xB4, 0xD8, // trig, id = 600
	0x3F, 0x3F, // cycle_id = 0xFFF
	0x3F, 0x3F, 0x3F, // triggers_count = 0x3FFF

	0x40, 0x4E, // counter = ticks
	0x3F, 0x3F, 0x3F, 0x3F, // 0xFFFFFF
	0x41, 0x4E, // counter = duration
	0x2A, 0x2A, 0x2A, 0x2A, // 0xAAAAAA
	0x4F, 0x7F, // 0x7F4F little-endian
	0x3F, 0x3F, 0x3F, 0x3F, // 0xFFFFFF

	0x41, 0x43, 0x3B, 0x1C, 0x2D, 0x0F, // some noise

	// test pre-jun10 additional bytes
	0x4E, 0x00, 0x00, 0x00, // record length
	0x00, 0x01, 0x00, 0x00, // type = cycle
	0x00, 0xE2, 0xB3, 0x4B, // Thu Apr  1 00:00:00 2010 (GMT)
	0x87, 0x65, 0x43, 0x21, // record id
	0x00, 0x00, 0x00, 0x00, // flags

	0xB4, 0xD8, // trig, id = 600
	0x3F, 0x3F, // cycle_id = 0xFFF
	0xFF, // extra byte before jun10
	0x3F, 0x3F, 0x3F, // triggers_count = 0x3FFF
	0x4F, 0x7F, // 0x7F4F little-endian
	0x3F, 0x3F, 0x3F, 0x3F, // 0xFFFFFF

	0x0F, 0x00, 0x00, 0x00, // event length
	0x12, 0x34, 0x56, 0x78, // event id
	0x00, 0x00, 0x00, 0x00, // event flag

	// some broken record
	0x0F, 0x00, 0x00, 0x00, // record length
	0x00, 0x01, 0x00, 0x00, // type = cycle
	0x8C, 0xCD, 0x2C, 0x4C, // Thu Jul  1 21:17:00 2010
	0x87, 0x65, 0x43, 0x21, // record id
	0x00, 0x00, 0x00, 0x00, // flags
	// unfortunately truncated right at the beginning

	0x40, 0x00, 0x00, 0x00, // record length
	0x00, 0x01, 0x00, 0x00, // type = cycle
	0x8C, 0xCD, 0x2C, 0x4C, // Thu Jul  1 21:17:00 2010
	0x87, 0x65, 0x43, 0x21, // record id
	0x00, 0x00, 0x00, 0x00, // flags

      	0x2C, 0x00, 0x00, 0x00, // event length
      	0x12, 0x34, 0x56, 0x78, // event id
      	0x00, 0x00, 0x00, 0x80, // event flag (EOC)

	0x00, 0x00, 0x00, 0x00, // event internals don't bother us too much
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,

	0x20, 0x00, 0x00, 0x00, // record length
	0x00, 0x01, 0x00, 0x00, // type = cycle
	0x8C, 0xCD, 0x2C, 0x4C, // Thu Jul  1 21:17:00 2010
	0x87, 0x65, 0x43, 0x21, // record id
	0x00, 0x00, 0x00, 0x00, // flags

      	0x0C, 0x00, 0x00, 0x00, // empty event
      	0x12, 0x34, 0x56, 0x78,
      	0x00, 0x00, 0x00, 0x00,
};

const int	PROP_EVENTS_COUNT = 1;
const int	RECORD_ENDS_COUNT = 3;

const char	tmp_filename[] = "temp.dat";

class TestPropLoadHook: public LoadHook
{
public:
	int	prop_data_calls;
	int	event_end_calls;

	TestPropLoadHook() : prop_data_calls(0), event_end_calls(0) {};

	void	handle_prop_data( const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id );
	void	handle_event_end();
};

void TestPropLoadHook::handle_prop_data(
	const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id
	)
{
	BOOST_REQUIRE(prop_data_calls < PROP_EVENTS_COUNT);

	BOOST_CHECK_EQUAL(int(end - begin), 1);
	BOOST_CHECK_EQUAL(dev_id, 20);
	BOOST_CHECK_EQUAL(*begin, 42);

	prop_data_calls++;
}

void	TestPropLoadHook::handle_event_end()
{
	BOOST_REQUIRE(event_end_calls < RECORD_ENDS_COUNT);

	event_end_calls++;
}

BOOST_AUTO_TEST_CASE( check_prop_reading )
{
	{
		std::ofstream	temp(tmp_filename,
				     std::ios_base::trunc | std::ios_base::binary);

		BOOST_CHECK(temp.is_open());

		temp.write(reinterpret_cast<const char*>(prop_data), sizeof(prop_data));

		temp.close();
	}

	{
		TestPropLoadHook	hook;

		loadfile(tmp_filename, hook);

		BOOST_CHECK_EQUAL(hook.prop_data_calls, PROP_EVENTS_COUNT);
		BOOST_CHECK_EQUAL(hook.event_end_calls, RECORD_ENDS_COUNT);
	}
}

BOOST_AUTO_TEST_SUITE_END()
