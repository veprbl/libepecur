#ifndef __PROCESSMAIN_HPP
#define __PROCESSMAIN_HPP

#include <vector>

#include <boost/numeric/ublas/matrix.hpp>

#include <TTree.h>

using namespace boost::numeric;

typedef std::vector< ublas::vector<double> >	vector_list_t;

struct process_result_t
{
	vector_list_t	i1;
	vector_list_t	i2;
};

void	Process( TTree *events, process_result_t *result );

#endif
