#include <boost/foreach.hpp>

#include <TEveBox.h>
#include <TEveManager.h>
#include <TEvePointSet.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TGLViewer.h>
#include <TEveViewer.h>

#include "ProcessVisualizer.hpp"

process_result_t vis_result;

TEveTrack*	make_track(track3d_t &t, TEveTrackPropagator* prop)
{
	TEveRecTrackD *rc = new TEveRecTrackD();
	rc->fV.Set(t.a(0), t.a(1), t.a(2));
	rc->fP.Set(t.b(0), t.b(1), t.b(2));
	TEveTrack* track = new TEveTrack(rc, prop);
	track->MakeTrack();
	return track;
}

void	ProcessVisualize()
{
	TEveManager::Create();

	TEvePointSetArray	*evp =
	    new TEvePointSetArray("Event points", "Event points");
	TEvePointSetArray	*chp =
	    new TEvePointSetArray("Chamber points", "Chamber points");
	evp->SetMarkerColor(kGreen);
	evp->InitBins("Event cause", 8, 0.5, 8.5);
	chp->SetMarkerColor(kCyan);
	chp->InitBins("Nothing", 1, -0.5, 0.5);
	TEveTrackList *list = new TEveTrackList();
	list->SetLineColor(kMagenta);
	list->SetName("Incident tracks");
	TEveTrackPropagator* prop = list->GetPropagator();
	prop->SetMaxR(2000);
	prop->SetMaxZ(4000);

	int	j = 0;
	BOOST_FOREACH(event_t &e, vis_result)
	{
		evp->Fill(e.i1.x, e.i1.y, e.i1.z, e.event_cause);
		chp->Fill(e.inc.a(0), e.inc.a(1), e.inc.a(2), 0);
		chp->Fill(e.l.a(0), e.l.a(1), e.l.a(2), 0);
		chp->Fill(e.r.a(0), e.r.a(1), e.r.a(2), 0);
		if ((++j % 1000) == 0)
		{
			TEveTrack	*track = make_track(e.inc, prop);
			track->SetLineColor(list->GetLineColor());
			list->AddElement(track);
			track = make_track(e.l, prop);
			track->SetLineColor(list->GetLineColor());
			list->AddElement(track);
			track = make_track(e.r, prop);
			track->SetLineColor(list->GetLineColor());
			list->AddElement(track);
		}
	}

	gEve->AddElement(list);
	gEve->AddElement(evp);
	gEve->AddElement(chp);
	gEve->Redraw3D(kTRUE);
}
