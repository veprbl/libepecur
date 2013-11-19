#include <boost/foreach.hpp>

#include <TEveBox.h>
#include <TEveManager.h>
#include <TEvePointSet.h>

#include "ProcessVisualizer.hpp"

process_result_t vis_result;

void	ProcessVisualize( int argc, char *argv[] )
{
	TEveManager::Create();

	TEvePointSetArray	*a = new TEvePointSetArray("a", "event points");
	a->InitBins("nothing", 8, 1, 8);
	BOOST_FOREACH(event_t &e, vis_result)
	{
		a->Fill(e.i1.x, e.i1.y, e.i1.z, e.event_cause);
	}

	gEve->AddElement(a);
	gEve->Redraw3D(kTRUE);
}
