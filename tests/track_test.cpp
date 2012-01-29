#include <vector>

#include <boost/test/unit_test.hpp>

#include <epecur/track.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE( track_test )

BOOST_AUTO_TEST_CASE( check_proper_iteration_func )
{
	const int	CHAMBERS_COUNT = 4;
	const int	count[CHAMBERS_COUNT] = { 3, 3, 3, 1 };
	unsigned char	x[CHAMBERS_COUNT];
	int		num_combinations = 1, combination_id = 0;

	for(int i = 0; i < CHAMBERS_COUNT; i++)
	{
		x[i] = 0;
		num_combinations *= count[i];
	}

	do
	{
		BOOST_REQUIRE(1 + combination_id <= num_combinations);

		uint hash = 0;

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

BOOST_AUTO_TEST_CASE( check_prop_recognize_track )
{
	vector< vector<wire_pos_t>* >	data
	{
		new vector<wire_pos_t>({ 1,2,3 }),
		new vector<wire_pos_t>({ 1,2,3 }),
		new vector<wire_pos_t>({ 3,4,5 }),
		new vector<wire_pos_t>({ 4 })
	};

	vector<double>	normal_pos({1, 2, 3, 4});

	prop_recognize_track(data, normal_pos);
}

BOOST_AUTO_TEST_SUITE_END()
