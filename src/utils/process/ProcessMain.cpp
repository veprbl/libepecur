#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "ProcessMain.hpp"
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

enum cham_group_t {prop_2nd, drift_left, drift_right};

double  Psi_L = 1.0447;
double  Psi_R = -1.0209;

struct track3d_t
{
	ublas::vector<double>	a, b;
};

template<cham_group_t cham_group>
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

	static ublas::matrix<double> m1(3, 3), m2(3, 3), A(3, 3);

	m1(0, 0) = 1; m1(1, 0) = 0;  m1(2, 0) = 0;
	m1(0, 1) = 0; m1(1, 1) = 0;  m1(2, 1) = 1;
	m1(0, 2) = 0; m1(1, 2) = -1; m1(2, 2) = 0;

	double Psi;
	if (cham_group == cham_group_t::drift_left)
	{
		Psi = Psi_L;
	}
	else if (cham_group == cham_group_t::drift_right)
	{
		Psi = Psi_R;
	}
	else if (cham_group == cham_group_t::prop_2nd)
	{
		Psi = 0;
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

	if (cham_group == cham_group_t::drift_left)
	{
		o(0) = 165.2;
		o(1) = 512.4;
		o(2) = 0.3;
	}
	else if (cham_group == cham_group_t::drift_right)
	{
		o(0) = 159.7;
		o(1) = -524.0;
		o(2) = -3.0;
	}
	else if (cham_group == cham_group_t::prop_2nd)
	{
		o(0) = -1486.0;
		o(1) = 0;
		o(2) = 2;
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

	return track3d_t{a, b};
}

void	find_intersection_points(
	const track3d_t &t1, const track3d_t &t2,
	intersection_t *i1, intersection_t *i2
	)
{
	auto cr = cross(t1.b, t2.b);
	double cr_norm = norm_2(cr);

	// Following code solves linear system
	// (t1.b, t2.b, cr) * x = t1.a - t2.a
	ublas::matrix<double>	B(3, 3);
	ublas::vector<double>	x;
	ublas::permutation_matrix<>	pm(B.size1());

	for(int row = 0; row < B.size1(); row++)
	{
		B(row, 0) = t1.b(row);
		B(row, 1) = t2.b(row);
		B(row, 2) = cr(row);
	}
	x = t1.a - t2.a; // put RHS into x

	int	singular = ublas::lu_factorize(B, pm);
	if(singular)
	{
		throw "lu_factorize()==0";
	}

	ublas::lu_substitute(B, pm, x);

	x(2) -= ublas::inner_prod(cr, t1.a - t2.a) / cr_norm;
	ublas::vector<double>	iv1 = t1.a - x(0) * t1.b;
	ublas::vector<double>	iv2 = t2.a + x(1) * t2.b;

	i1->x = iv1(0); i1->y = iv1(1); i1->z = iv1(2);
	i2->x = iv2(0); i2->y = iv2(1); i2->z = iv2(2);
}

void	Process( TTree *events, process_result_t *result, intersection_set_t *s )
{
	track_group_t	tg_F2X, tg_F2Y, tg_LX, tg_LY, tg_RX, tg_RY;

	events->GetBranch("t2X_track_count")->SetAddress(&tg_F2X.track_count);
	events->GetBranch("t2Y_track_count")->SetAddress(&tg_F2Y.track_count);
	events->GetBranch("t3X_track_count")->SetAddress(&tg_LX.track_count);
	events->GetBranch("t3Y_track_count")->SetAddress(&tg_LY.track_count);
	events->GetBranch("t4X_track_count")->SetAddress(&tg_RX.track_count);
	events->GetBranch("t4Y_track_count")->SetAddress(&tg_RY.track_count);

	tg_F2X.c0_br = events->GetBranch("t2X_c0");
	tg_F2X.c1_br = events->GetBranch("t2X_c1");
	tg_F2Y.c0_br = events->GetBranch("t2Y_c0");
	tg_F2Y.c1_br = events->GetBranch("t2Y_c1");

	tg_LX.c0_br = events->GetBranch("t3X_c0");
	tg_LX.c1_br = events->GetBranch("t3X_c1");
	tg_LY.c0_br = events->GetBranch("t3Y_c0");
	tg_LY.c1_br = events->GetBranch("t3Y_c1");

	tg_RX.c0_br = events->GetBranch("t4X_c0");
	tg_RX.c1_br = events->GetBranch("t4X_c1");
	tg_RY.c0_br = events->GetBranch("t4Y_c0");
	tg_RY.c1_br = events->GetBranch("t4Y_c1");

	bool cond;
	for(int i = -1; i < events->GetEntries(); i++)
	{
		events->GetBranch("t2X_track_count")->GetEntry(i);
		events->GetBranch("t2Y_track_count")->GetEntry(i);
		events->GetBranch("t3X_track_count")->GetEntry(i);
		events->GetBranch("t3Y_track_count")->GetEntry(i);
		events->GetBranch("t4X_track_count")->GetEntry(i);
		events->GetBranch("t4Y_track_count")->GetEntry(i);
		cond = (tg_LX.track_count == 1) && (tg_LY.track_count == 1)
		    && (tg_RX.track_count == 1) && (tg_RY.track_count == 1)
		    && (tg_F2X.track_count == 1) && (tg_F2Y.track_count == 1);
		if (cond)
		{
			track3d_t	t_L = make_track<cham_group_t::drift_left>(i, tg_LX, tg_LY);
			track3d_t	t_R = make_track<cham_group_t::drift_right>(i, tg_RX, tg_RY);
			track3d_t	t_F2 = make_track<cham_group_t::prop_2nd>(i, tg_F2X, tg_F2Y);

			find_intersection_points(t_L, t_R, &s->i_lr, &s->i_rl);
			s->br_lr->Fill();
			s->br_rl->Fill();
		}
	}
}
