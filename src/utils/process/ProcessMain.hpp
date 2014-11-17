#ifndef __PROCESSMAIN_HPP
#define __PROCESSMAIN_HPP

#include <vector>

#include <boost/numeric/ublas/matrix.hpp>

#include <TTree.h>
#include <TBranch.h>

#include <epecur/geometry.hpp>

using namespace boost::numeric;

struct intersection_t
{
	double	x, y, z;
};

struct track3d_t
{
	ublas::vector<double>	a, b;
};

struct intersection_set_t
{
	intersection_t	i_lr, i_rl, i_f2r, i_f2l, i_rf2, i_lf2;
	TBranch	*br_lr, *br_rl, *br_f2r, *br_f2l, *br_rf2, *br_lf2;
};

TTree*	Process( TTree *events, TTree *cycle_efficiency, Geometry &geom, double central_momentum, intersection_set_t *s );

#endif
