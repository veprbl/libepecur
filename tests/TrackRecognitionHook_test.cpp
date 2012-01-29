#include <iostream>
#include <sstream>

#include <boost/test/unit_test.hpp>

#include <epecur/types.hpp>
#include <epecur/geometry.hpp>
#include <epecur/TrackRecognitionHook.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE( TrackRecognitionHook_test )

const string	test_geometry = 
	"// F1plane1.normal_pos = 0\n"
	"// F1plane2.normal_pos = 100\n"
	"// F1plane3.normal_pos = 200\n"
	"// F1X1\n"
	"{0, 1, 0}\n" // device_id = 0
	"// F1X2\n"
	"{1, 1, 0}\n" // device_id = 1
	"// F1X3\n"
	"{2, 1, 0}\n" // device_id = 2
	"// F1Y1\n"
	"{3, 1, 0}\n" // device_id = 3
	"// F1Y2\n"
	"{4, 1, 0}\n" // device_id = 4
	"// F1Y3\n"
	"{5, 1, 0}\n" // device_id = 5
;

BOOST_AUTO_TEST_CASE( check_basic_track_recognition )
{
	stringstream	test_geometry_stream(test_geometry, ios_base::in);
	Geometry	geometry(test_geometry_stream);
	TrackRecognitionHook	hook(geometry);

	const vector<wire_id_t>	wires_f1x1 {1, 50};
	const vector<wire_id_t>	wires_f1x2 {2, 50};
	const vector<wire_id_t>	wires_f1x3 {3, 50};

	hook.handle_prop_data(&(*wires_f1x1.begin()), &(*wires_f1x1.end()), 0);
	hook.handle_prop_data(&(*wires_f1x2.begin()), &(*wires_f1x2.end()), 1);
	hook.handle_prop_data(&(*wires_f1x3.begin()), &(*wires_f1x3.end()), 2);

	hook.handle_event_end();

	BOOST_REQUIRE_EQUAL(hook.last_tracks[1][DEV_AXIS_X].size(), 2);
	BOOST_REQUIRE_EQUAL(hook.last_tracks[1][DEV_AXIS_Y].size(), 0);

	vector<track_info_t>	&track = hook.last_tracks[1][DEV_AXIS_X];

	BOOST_CHECK((track[0].c0 == 50 && track[0].c1 == 0 && track[1].c0 == 1 && track[1].c1 == 0.01)
		    || (track[0].c0 == 1 && track[0].c1 == 0.01 && track[1].c0 == 50 && track[1].c1 == 0));
}

BOOST_AUTO_TEST_CASE( check_big_chisq_tracks_filter )
{
	stringstream	test_geometry_stream(test_geometry, ios_base::in);
	Geometry	geometry(test_geometry_stream);
	TrackRecognitionHook	hook(geometry, 1e-10);

	const vector<wire_id_t>	wires_f1x1 {90};
	const vector<wire_id_t>	wires_f1x2 {10};
	const vector<wire_id_t>	wires_f1x3 {90};

	hook.handle_prop_data(&(*wires_f1x1.begin()), &(*wires_f1x1.end()), 0);
	hook.handle_prop_data(&(*wires_f1x2.begin()), &(*wires_f1x2.end()), 1);
	hook.handle_prop_data(&(*wires_f1x3.begin()), &(*wires_f1x3.end()), 2);

	hook.handle_event_end();

	BOOST_REQUIRE_EQUAL(hook.last_tracks[1][DEV_AXIS_X].size(), 0);
	BOOST_REQUIRE_EQUAL(hook.last_tracks[1][DEV_AXIS_Y].size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
