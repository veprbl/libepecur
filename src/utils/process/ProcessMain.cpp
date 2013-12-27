#include <cmath>

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

struct plane3d_t
{
	ublas::vector<double>	a, norm;
};

template<cham_group_t cham_group>
track3d_t make_track( int event_id, track_group_t &tg_X, track_group_t &tg_Y )
{
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
		o(2) = -2.8;
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

	return track3d_t({a, b});
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

	for(unsigned int row = 0; row < B.size1(); row++)
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

ublas::vector<double>	intersect_with_plane(
	const track3d_t &t, const plane3d_t &p
	)
{
	double	d = ublas::inner_prod(t.b, p.norm);
	double	x = ublas::inner_prod(p.a - t.a, p.norm) / d;
	return t.a + t.b * x;
}

double	calc_theta(track3d_t track)
{
	double	cos_theta =
	    ublas::inner_prod(track.b, ublas::unit_vector<double>(3, 0))
	    / norm_2(track.b);
	return acos(cos_theta);
}

double	calc_phi(track3d_t track)
{
	return atan2(
		ublas::inner_prod(track.b, ublas::unit_vector<double>(3, 1)),
		ublas::inner_prod(track.b, ublas::unit_vector<double>(3, 2))
		);
}

TTree*	Process( TTree *events, Geometry &geom, double central_momentum, process_result_t *result, intersection_set_t *s )
{
	TTree	*events_new;
	int32_t	event_cause;
	double	theta_l, theta_r, phi_l, phi_r, beam_momentum, incident_momentum;
	track_group_t	tg_F1X, tg_F1Y, tg_F2X, tg_F2Y, tg_LX, tg_LY, tg_RX, tg_RY;
	track_group_t	tg_F1X_new, tg_F1Y_new, tg_F2X_new, tg_F2Y_new, tg_LX_new, tg_LY_new, tg_RX_new, tg_RY_new;
	const double	F1_length = geom.normal_pos[1][DEV_AXIS_X].back();

	events->GetBranch("event_cause")->SetAddress(&event_cause);
	events->GetBranch("t1X_track_count")->SetAddress(&tg_F1X.track_count);
	events->GetBranch("t1Y_track_count")->SetAddress(&tg_F1Y.track_count);
	events->GetBranch("t2X_track_count")->SetAddress(&tg_F2X.track_count);
	events->GetBranch("t2Y_track_count")->SetAddress(&tg_F2Y.track_count);
	events->GetBranch("t3X_track_count")->SetAddress(&tg_LX.track_count);
	events->GetBranch("t3Y_track_count")->SetAddress(&tg_LY.track_count);
	events->GetBranch("t4X_track_count")->SetAddress(&tg_RX.track_count);
	events->GetBranch("t4Y_track_count")->SetAddress(&tg_RY.track_count);

	tg_F1X.c0_ptr = &tg_F1X.c0;
	tg_F1X.c1_ptr = &tg_F1X.c1;
	tg_F1X.hits_count_ptr = &tg_F1X.hits_count;
	tg_F1Y.c0_ptr = &tg_F1Y.c0;
	tg_F1Y.c1_ptr = &tg_F1Y.c1;
	tg_F1Y.hits_count_ptr = &tg_F1Y.hits_count;

	tg_F2X.c0_ptr = &tg_F2X.c0;
	tg_F2X.c1_ptr = &tg_F2X.c1;
	tg_F2X.hits_count_ptr = &tg_F2X.hits_count;
	tg_F2Y.c0_ptr = &tg_F2Y.c0;
	tg_F2Y.c1_ptr = &tg_F2Y.c1;
	tg_F2Y.hits_count_ptr = &tg_F2Y.hits_count;

	tg_LX.c0_ptr = &tg_LX.c0;
	tg_LX.c1_ptr = &tg_LX.c1;
	tg_LX.hits_count_ptr = &tg_LX.hits_count;
	tg_LY.c0_ptr = &tg_LY.c0;
	tg_LY.c1_ptr = &tg_LY.c1;
	tg_LY.hits_count_ptr = &tg_LY.hits_count;

	tg_RX.c0_ptr = &tg_RX.c0;
	tg_RX.c1_ptr = &tg_RX.c1;
	tg_RX.hits_count_ptr = &tg_RX.hits_count;
	tg_RY.c0_ptr = &tg_RY.c0;
	tg_RY.c1_ptr = &tg_RY.c1;
	tg_RY.hits_count_ptr = &tg_RY.hits_count;

	events->SetBranchAddress("t1X_c0", &tg_F1X.c0_ptr);
	events->SetBranchAddress("t1X_c1", &tg_F1X.c1_ptr);
	events->SetBranchAddress("t1X_hits_count", &tg_F1X.hits_count_ptr);
	events->SetBranchAddress("t1Y_c0", &tg_F1Y.c0_ptr);
	events->SetBranchAddress("t1Y_c1", &tg_F1Y.c1_ptr);
	events->SetBranchAddress("t1Y_hits_count", &tg_F1Y.hits_count_ptr);

	events->SetBranchAddress("t2X_c0", &tg_F2X.c0_ptr);
	events->SetBranchAddress("t2X_c1", &tg_F2X.c1_ptr);
	events->SetBranchAddress("t2X_hits_count", &tg_F2X.hits_count_ptr);
	events->SetBranchAddress("t2Y_c0", &tg_F2Y.c0_ptr);
	events->SetBranchAddress("t2Y_c1", &tg_F2Y.c1_ptr);
	events->SetBranchAddress("t2Y_hits_count", &tg_F2Y.hits_count_ptr);

	events->SetBranchAddress("t3X_c0", &tg_LX.c0_ptr);
	events->SetBranchAddress("t3X_c1", &tg_LX.c1_ptr);
	events->SetBranchAddress("t3X_hits_count", &tg_LX.hits_count_ptr);
	events->SetBranchAddress("t3Y_c0", &tg_LY.c0_ptr);
	events->SetBranchAddress("t3Y_c1", &tg_LY.c1_ptr);
	events->SetBranchAddress("t3Y_hits_count", &tg_LY.hits_count_ptr);

	events->SetBranchAddress("t4X_c0", &tg_RX.c0_ptr);
	events->SetBranchAddress("t4X_c1", &tg_RX.c1_ptr);
	events->SetBranchAddress("t4X_hits_count", &tg_RX.hits_count_ptr);
	events->SetBranchAddress("t4Y_c0", &tg_RY.c0_ptr);
	events->SetBranchAddress("t4Y_c1", &tg_RY.c1_ptr);
	events->SetBranchAddress("t4Y_hits_count", &tg_RY.hits_count_ptr);

	// select branches to work with
	events->SetBranchStatus("*", 0);
	events->SetBranchStatus("event_cause", 1);
	events->SetBranchStatus("*_track_count", 1);
	events->SetBranchStatus("*_hits_count", 1);
	events->SetBranchStatus("*_c0", 1);
	events->SetBranchStatus("*_c1", 1);
	// clone tree headers
	// this also copies branches addresses
	events_new = events->CloneTree(0);

	s->br_lr = events_new->Branch("LR", &s->i_lr, "LR_x/D:LR_y/D:LR_z/D");
	s->br_rl = events_new->Branch("RL", &s->i_rl, "RL_x/D:RL_y/D:RL_z/D");
	s->br_f2r = events_new->Branch("F2R", &s->i_f2r, "F2R_x/D:F2R_y/D:F2R_z/D");
	s->br_f2l = events_new->Branch("F2L", &s->i_f2l, "F2L_x/D:F2L_y/D:F2L_z/D");
	s->br_rf2 = events_new->Branch("RF2", &s->i_rf2, "RF2_x/D:RF2_y/D:RF2_z/D");
	s->br_lf2 = events_new->Branch("LF2", &s->i_lf2, "LF2_x/D:LF2_y/D:LF2_z/D");

	events_new->Branch("LP", NULL, "LP_x/D:LP_y/D:LP_z/D");
	events_new->Branch("RP", NULL, "RP_x/D:RP_y/D:RP_z/D");

	events_new->Branch("theta_l", NULL, "theta_l/D")->SetAddress(&theta_l);
	events_new->Branch("theta_r", NULL, "theta_r/D")->SetAddress(&theta_r);
	events_new->Branch("phi_l", NULL, "phi_l/D")->SetAddress(&phi_l);
	events_new->Branch("phi_r", NULL, "phi_r/D")->SetAddress(&phi_r);
	events_new->Branch("beam_momentum", NULL, "beam_momentum/D")
	    ->SetAddress(&beam_momentum);
	events_new->Branch("incident_momentum", NULL, "incident_momentum/D")
	    ->SetAddress(&incident_momentum);

	for(int i = 0; i < events->GetEntries(); i++)
	{
		track3d_t	t_L, t_R, t_F2;
		events->GetEntry(i);

		bool	left_arm = (tg_LX.track_count == 1) && (tg_LY.track_count == 1);
		bool	right_arm = (tg_RX.track_count == 1) && (tg_RY.track_count == 1);
		bool	incident =
		    (tg_F1X.track_count == 1) && (tg_F1Y.track_count == 1) &&
		    (tg_F2X.track_count == 1) && (tg_F2Y.track_count == 1);

		if (left_arm)
		{
			t_L = make_track<cham_group_t::drift_left>(i, tg_LX, tg_LY);
			theta_l = calc_theta(t_L);
			phi_l = calc_phi(t_L);
		}
		else
		{
			theta_l = NAN;
			phi_l = NAN;
		}
		if (right_arm)
		{
			t_R = make_track<cham_group_t::drift_right>(i, tg_RX, tg_RY);
			theta_r = calc_theta(t_R);
			phi_r = calc_phi(t_R);
		}
		else
		{
			theta_r = NAN;
			phi_r = NAN;
		}

		if (left_arm && right_arm && incident)
		{
			t_F2 = make_track<cham_group_t::prop_2nd>(i, tg_F2X, tg_F2Y);

			double	F1_x = tg_F1X.c0[0] + F1_length * tg_F1X.c1[0];
			const double	DISPERSION = (1.0 / 55) * 0.01; // 55 mm/%
			beam_momentum = (1 + F1_x * DISPERSION) * central_momentum;

			find_intersection_points(t_L, t_R, &s->i_lr, &s->i_rl);
			find_intersection_points(t_F2, t_L, &s->i_f2l, &s->i_lf2);
			find_intersection_points(t_F2, t_R, &s->i_f2r, &s->i_rf2);

			result->push_back(event_t({event_cause, t_F2, t_L, t_R, s->i_lr, s->i_rl}));

			const double	TARGET_START = -130; // mm
			double	z = (s->i_f2l.x + s->i_lf2.x) / 2 - TARGET_START; // mm
			const double	dE_over_dx = 4; // MeV g^-1 cm^2
			const double	lih2_density = 0.0708; // g cm^-3
			incident_momentum =
			    beam_momentum - dE_over_dx * lih2_density * (z/10);

			plane3d_t	plane;
			plane.a = ublas::vector<double>(3);
			plane.a(0) = 0.0;    plane.a(1) = 0.0;    plane.a(2) = 0.0;
			plane.norm = ublas::vector<double>(3);
			plane.norm(0) = 0.0; plane.norm(1) = 1.0; plane.norm(2) = 0.0;

			ublas::vector<double>	lp = intersect_with_plane(t_L, plane);
			ublas::vector<double>	rp = intersect_with_plane(t_R, plane);

			events_new->GetBranch("LP")->SetAddress(&lp.data()[0]);
			events_new->GetBranch("RP")->SetAddress(&rp.data()[0]);

			events_new->Fill();
		}
	}

	return events_new;
}
