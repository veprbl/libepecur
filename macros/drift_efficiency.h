#ifndef __DRIFT_EFFECTIVITY_H
#define __DRIFT_EFFECTIVITY_H

#include <TH1F.h>
#include <TTree.h>

TH1F*   make_drift_efficiency_hist(char arm, char axis, TTree *events);

#endif
