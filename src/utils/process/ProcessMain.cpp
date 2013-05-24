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

void	Process( TTree *events )
{
	track_group_t	tg_LX, tg_LY;

    events->GetBranch("t3X_track_count")->SetAddress(&tg_LX.track_count);
    events->GetBranch("t3Y_track_count")->SetAddress(&tg_LY.track_count);
	int i = -1, j = 0;
	bool cond;
	do
	{
		i++;
		events->GetEntry(i);
		cond = (tg_LX.track_count == 0) || (tg_LY.track_count == 0);
		if (!cond)
		{
			j++;
		}
	}
	while(cond || (j != 2));

	tg_LX.c0_br = events->GetBranch("t3X_c0");
	tg_LX.c1_br = events->GetBranch("t3X_c1");
	tg_LX.c0.resize(tg_LX.track_count);
	tg_LX.c1.resize(tg_LX.track_count);
	tg_LX.c0_br->SetAddress(tg_LX.c0.data());
	tg_LX.c1_br->SetAddress(tg_LX.c1.data());
	tg_LX.c0_br->GetEntry(i);
	tg_LX.c1_br->GetEntry(i);

	tg_LY.c0_br = events->GetBranch("t3Y_c0");
	tg_LY.c1_br = events->GetBranch("t3Y_c1");
	tg_LY.c0.resize(tg_LY.track_count);
	tg_LY.c1.resize(tg_LY.track_count);
	tg_LY.c0_br->SetAddress(tg_LY.c0.data());
	tg_LY.c1_br->SetAddress(tg_LY.c1.data());
	tg_LY.c0_br->GetEntry(i);
	tg_LY.c1_br->GetEntry(i);

	cout << tg_LX.track_count << "\t" << tg_LY.track_count << endl;

	double	Psi_L = 1.0447;
	double	Psi_R = -1.0209;
	ublas::matrix<double> m1(3, 3), m2(3, 3), A(3, 3);

	m1(0, 0) = 1; m1(1, 0) = 0; m1(2, 0) = 0;
	m1(0, 1) = 0; m1(1, 1) = 0; m1(2, 1) = -1;
	m1(0, 2) = 0; m1(1, 2) = 1; m1(2, 2) = 0;

	m2(0, 0) = cos(Psi_L);  m2(1, 0) = 0; m2(2, 0) = sin(Psi_L);
	m2(0, 1) = 0;           m2(1, 1) = 1; m2(2, 1) = 0;
	m2(0, 2) = -sin(Psi_L); m2(1, 2) = 0; m2(2, 2) = cos(Psi_L);

	A = ublas::prod(m1, m2);

	ublas::vector<double> o_L(3), a(3), b(3);

	o_L(0) = 165.2;
	o_L(1) = 512.4;
	o_L(2) = 0.3;

	b(0) = 1;
	b(1) = tg_LY.c1[0];
	b(2) = tg_LX.c1[0];
	b = ublas::prod(A, b);

	ublas::unit_vector<double> xk(3, 1);
	ublas::unit_vector<double> xl(3, 2);
	a = o_L + ublas::prod(A, xk) * tg_LY.c0[0] + ublas::prod(A, xl) * tg_LX.c0[0];

	cout << "X\t" << tg_LX.c0[0] << "\t" << tg_LX.c1[0] << endl;
	cout << "Y\t" << tg_LY.c0[0] << "\t" << tg_LY.c1[0] << endl;

	cout << a << endl;
	cout << b << endl;

	auto	norm_a = ublas::norm_2(a);
	auto	norm_b = ublas::norm_2(b);
	auto	inner_ab = ublas::inner_prod(a, b);

	cout << sqrt(norm_a * norm_a - inner_ab * inner_ab/(norm_b * norm_b)) << endl;
}
