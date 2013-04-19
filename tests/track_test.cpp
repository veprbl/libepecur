#include <vector>

#include <boost/test/unit_test.hpp>

#include <epecur/track.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE( track_test )

BOOST_AUTO_TEST_CASE( check_proper_iteration_func )
{
	const int	CHAMBERS_COUNT = 4;
	const int	count[CHAMBERS_COUNT] = { 3, 3, 3, 1 };
	vector<wire_pos_ptr_t>	x(CHAMBERS_COUNT);
	int		num_combinations = 1, combination_id = 0;

	for(int i = 0; i < CHAMBERS_COUNT; i++)
	{
		x[i] = 0;
		num_combinations *= count[i];
	}

	do
	{
		BOOST_REQUIRE(1 + combination_id <= num_combinations);

		wire_pos_ptr_t	hash = 0;

		for(int i = CHAMBERS_COUNT - 1; i >= 0; i--)
		{
			hash += x[i];

			if (i != 0)
			{
				hash *= count[i];
			}
		}

		BOOST_CHECK_EQUAL(combination_id, hash);

		combination_id++;
	}
	while(next(x, count, CHAMBERS_COUNT ));

	BOOST_CHECK_EQUAL(combination_id, num_combinations);
}

BOOST_AUTO_TEST_CASE( check_recognize_track )
{
	vector<wire_pos_t>	v1;
	v1.push_back(1); v1.push_back(2); v1.push_back(3);
	vector<wire_pos_t>	v2;
	v2.push_back(3); v2.push_back(4); v2.push_back(5);
	vector<wire_pos_t>	v3;
	v3.push_back(4);

	vector< vector<wire_pos_t>* >	data;
	data.push_back(&v1);
	data.push_back(&v1);
	data.push_back(&v2);
	data.push_back(&v3);

	vector<double>	normal_pos;
	normal_pos.push_back(1);
	normal_pos.push_back(2);
	normal_pos.push_back(3);
	normal_pos.push_back(4);

	recognize_track(data, normal_pos);
}

BOOST_AUTO_TEST_CASE( check_recognize_all_tracks_prop )
{
	vector<wire_pos_t>	v1;
	v1.push_back(1); v1.push_back(50);
	vector<wire_pos_t>	v2;
	v2.push_back(1); v2.push_back(2); v2.push_back(50);
	vector<wire_pos_t>	v3;
	v3.push_back(50);
	vector<wire_pos_t>	v4;
	v4.push_back(4); v4.push_back(50);

	vector< vector<wire_pos_t>* >	data;
	data.push_back(&v1);
	data.push_back(&v2);
	data.push_back(&v3);
	data.push_back(&v4);

	vector<double>	normal_pos;
	normal_pos.push_back(1);
	normal_pos.push_back(2);
	normal_pos.push_back(3);
	normal_pos.push_back(4);

	vector<track_info_t>	tracks = recognize_all_tracks<track_type_t::prop>(data, normal_pos);

	BOOST_REQUIRE_EQUAL(tracks.size(), 2u);
	BOOST_CHECK_EQUAL(tracks[0].c0, 50);
	BOOST_CHECK_EQUAL(tracks[0].c1, 0);
	BOOST_CHECK_EQUAL(tracks[1].c0, 0);
	BOOST_CHECK_EQUAL(tracks[1].c1, 1);
}

BOOST_AUTO_TEST_SUITE_END()
