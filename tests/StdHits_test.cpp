#include <fstream>
#include <cmath>

#include <boost/test/unit_test.hpp>

#include <epecur/geometry.hpp>
#include <epecur/StdHits.hpp>

#define L(a) (sizeof(a) / sizeof(a[0]))

BOOST_AUTO_TEST_SUITE(StdHits_test)

BOOST_AUTO_TEST_CASE(check_basic_prop_and_drift)
{
	std::ifstream	f(PROJECT_SOURCE_DIR "/contrib/geom_apr10.c", ios::in);
	Geometry	g(f);
	StdHits	hook(g, NULL);

	wire_id_t	wires[] = {1, 3, 4, 6};
	hook.handle_event_start();
	hook.handle_prop_data(wires, wires + L(wires), 3); // dev_id=3
	hook.handle_event_end();
	BOOST_REQUIRE_EQUAL(hook.last_event[1].size(), L(wires)); // translates to chamber_id=1

	hook.handle_event_start();
	hook.handle_prop_data(wires, wires + L(wires), 3);
	hook.handle_prop_data(wires, wires + L(wires), 4);
	hook.handle_event_end();
	BOOST_REQUIRE_EQUAL(hook.last_event[1].size(), 2 * L(wires));

	std::vector<wire_id_t> drift_wires = {1, 2, 3, 4};
	std::vector<uint16_t> drift_times = {10, 20, 1, 80};
	hook.handle_event_start();
	hook.handle_drift_data(drift_wires, drift_times, 122);
	hook.handle_event_end();
	BOOST_REQUIRE_EQUAL(hook.last_event[24].size(), drift_wires.size());

	hook.handle_event_start();
	hook.handle_event_end();
	BOOST_REQUIRE_EQUAL(hook.last_event[1].size(), 0); // should be empty
	BOOST_REQUIRE_EQUAL(hook.last_event[24].size(), 0);
}

BOOST_AUTO_TEST_CASE(check_drift_calib)
{
	std::ifstream	f(PROJECT_SOURCE_DIR "/contrib/geom_apr10.c", ios::in);
	Geometry	g(f);
	StdHits::calibration_curve_t	calibs;
	vector<double> curve(100);
	for(int i = 0; i < 100; i++)
	{
		curve[i] = min(i/100.0, 1.0);
	}
	calibs[24] = curve;
	StdHits	hook(g, &calibs);

	std::vector<wire_id_t> drift_wires = {1, 20};
	std::vector<uint16_t> drift_times = {0, 3};
	hook.handle_event_start();
	hook.handle_drift_data(drift_wires, drift_times, 122);
	hook.handle_event_end();
	BOOST_REQUIRE_EQUAL(hook.last_event[24].size(), 2 * drift_wires.size());
}

BOOST_AUTO_TEST_SUITE_END()
