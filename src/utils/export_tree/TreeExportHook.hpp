#ifndef __TREEEXPORTHOOK_HPP
#define __TREEEXPORTHOOK_HPP

#include <TTree.h>

#include <epecur/types.hpp>
#include <epecur/TrackRecognitionHook.hpp>

class TreeExportHook : public TrackRecognitionHook
{
private:

	TTree	tree;
	double	c0, c1;
	group_id_t	group_id;

public:

	TreeExportHook( Geometry &g, double max_chisq = -1 );
	virtual void	handle_event_end();
};

#endif
