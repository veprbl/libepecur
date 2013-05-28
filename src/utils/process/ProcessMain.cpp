#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "../export_tree/TreeExportHook.hpp"

using namespace boost::numeric;

template<typename T>
ublas::vector<T>    cross( const ublas::vector<T> &a, const ublas::vector<T> &b )
{
	BOOST_ASSERT(a.size() == 3);
	BOOST_ASSERT(b.size() == 3);

	ublas::vector<T>	result(3);
	result(0) = a(1) * b(2) - a(2) * b(1);
	result(1) = a(2) * b(0) - a(0) * b(2);
	result(2) = a(0) * b(1) - a(1) * b(0);
	return result;
}

enum left_right_t {left, right};

double  Psi_L = 1.0447;
double  Psi_R = -1.0209;

struct track3d_t
{
	ublas::vector<double>	a, b;
};

template<left_right_t side>
track3d_t make_track( int event_id, track_group_t &tg_X, track_group_t &tg_Y )
{
	tg_X.c0.resize(tg_X.track_count);
	tg_X.c1.resize(tg_X.track_count);
	tg_X.c0_br->SetAddress(tg_X.c0.data());
	tg_X.c1_br->SetAddress(tg_X.c1.data());
	tg_X.c0_br->GetEntry(event_id);
	tg_X.c1_br->GetEntry(event_id);

	tg_Y.c0.resize(tg_Y.track_count);
	tg_Y.c1.resize(tg_Y.track_count);
	tg_Y.c0_br->SetAddress(tg_Y.c0.data());
	tg_Y.c1_br->SetAddress(tg_Y.c1.data());
	tg_Y.c0_br->GetEntry(event_id);
	tg_Y.c1_br->GetEntry(event_id);

	cout << tg_X.track_count << "\t" << tg_Y.track_count << endl;

	static ublas::matrix<double> m1(3, 3), m2(3, 3), A(3, 3);

	m1(0, 0) = 1; m1(1, 0) = 0;  m1(2, 0) = 0;
	m1(0, 1) = 0; m1(1, 1) = 0;  m1(2, 1) = 1;
	m1(0, 2) = 0; m1(1, 2) = -1; m1(2, 2) = 0;

	double Psi;
	if (side == left_right_t::left)
	{
		Psi = Psi_L;
	}
	else if (side == left_right_t::right)
	{
		Psi = Psi_R;
	}
	else
	{
		throw;
	}

	m2(0, 0) = cos(Psi); m2(1, 0) = 0; m2(2, 0) = -sin(Psi);
	m2(0, 1) = 0;        m2(1, 1) = 1; m2(2, 1) = 0;
	m2(0, 2) = sin(Psi); m2(1, 2) = 0; m2(2, 2) = cos(Psi);

	A = ublas::prod(m1, m2);

	ublas::vector<double> o(3), a(3), b(3);

	if (side == left_right_t::left)
	{
		o(0) = 165.2;
		o(1) = 512.4;
		o(2) = 0.3;
	}
	else if (side == left_right_t::right)
	{
		o(0) = 159.7;
		o(1) = -524.0;
		o(2) = -3.0;
	}
	else
	{
		throw;
	}

	b(0) = 1;
	b(1) = tg_Y.c1[0];
	b(2) = tg_X.c1[0];
	b = ublas::prod(A, b);

	ublas::unit_vector<double> xk(3, 1);
	ublas::unit_vector<double> xl(3, 2);
	a = o + ublas::prod(A, xk) * tg_Y.c0[0] + ublas::prod(A, xl) * tg_X.c0[0];

	cout << "X\t" << tg_X.c0[0] << "\t" << tg_X.c1[0] << endl;
	cout << "Y\t" << tg_Y.c0[0] << "\t" << tg_Y.c1[0] << endl;

	cout << a << endl;
	cout << b << endl;

	auto	norm_a = ublas::norm_2(a);
	auto	norm_b = ublas::norm_2(b);
	auto	inner_ab = ublas::inner_prod(a, b);

	cout << sqrt(norm_a * norm_a - inner_ab * inner_ab/(norm_b * norm_b)) << endl;

	return track3d_t{a, b};
}

void	Process( TTree *events )
{
	track_group_t	tg_LX, tg_LY, tg_RX, tg_RY;

    events->GetBranch("t3X_track_count")->SetAddress(&tg_LX.track_count);
    events->GetBranch("t3Y_track_count")->SetAddress(&tg_LY.track_count);
    events->GetBranch("t4X_track_count")->SetAddress(&tg_RX.track_count);
    events->GetBranch("t4Y_track_count")->SetAddress(&tg_RY.track_count);
	int i = -1, j = 0;
	bool cond;
	do
	{
		i++;
		events->GetEntry(i);
		cond = (tg_LX.track_count != 1) || (tg_LY.track_count != 1)
		    || (tg_RX.track_count != 1) || (tg_RY.track_count != 1);
		if (!cond)
		{
			j++;
		}
	}
	while(cond || (j != 2));

	tg_LX.c0_br = events->GetBranch("t3X_c0");
	tg_LX.c1_br = events->GetBranch("t3X_c1");
	tg_LY.c0_br = events->GetBranch("t3Y_c0");
	tg_LY.c1_br = events->GetBranch("t3Y_c1");

	tg_RX.c0_br = events->GetBranch("t4X_c0");
	tg_RX.c1_br = events->GetBranch("t4X_c1");
	tg_RY.c0_br = events->GetBranch("t4Y_c0");
	tg_RY.c1_br = events->GetBranch("t4Y_c1");

	track3d_t	t_L = make_track<left_right_t::left>(i, tg_LX, tg_LY);
	track3d_t	t_R = make_track<left_right_t::right>(i, tg_RX, tg_RY);

	auto cr = cross(t_L.b, t_R.b);
	double cr_norm = norm_2(cr);
	cout << ublas::inner_prod(cr, t_L.a - t_R.a) / cr_norm << endl;
}