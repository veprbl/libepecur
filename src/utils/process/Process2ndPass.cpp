#include <iostream>

#include <TH1F.h>
#include <TTree.h>

#include "macros/drift_efficiency.h"

#include "Process2ndPass.hpp"

double	get_efficiency(TH1F *h, double theta)
{
	int	bin = h->FindBin(theta);
	if (bin < 0)
	{
		std::cerr << "bin not found" << std::endl;
	}
	return h->GetBinContent(bin);
}

TTree*	Process2ndPass(TTree *events_new)
{
	TTree	*efficiency_tree = new TTree("efficiency", "");
	double	theta_l, theta_r;
	double	efficiency_l, efficiency_r;

	events_new->ResetBranchAddresses();

	TH1F	*h_lx = make_drift_efficiency_hist('l', 'X', events_new);
	TH1F	*h_ly = make_drift_efficiency_hist('l', 'Y', events_new);
	TH1F	*h_rx = make_drift_efficiency_hist('r', 'X', events_new);
	TH1F	*h_ry = make_drift_efficiency_hist('r', 'Y', events_new);

	efficiency_tree->Branch("efficiency_l", &efficiency_l, "efficiency_l/D");
	efficiency_tree->Branch("efficiency_r", &efficiency_r, "efficiency_r/D");

	events_new->GetBranch("theta_l")->SetAddress(&theta_l);
	events_new->GetBranch("theta_r")->SetAddress(&theta_r);

	std::cerr << events_new->GetEntries() << std::endl;

	for(int i = 0; i < events_new->GetEntries(); i++)
	{
		events_new->GetEntry(i);

		efficiency_l = get_efficiency(h_lx, theta_l) * get_efficiency(h_ly, theta_l);
		efficiency_r = get_efficiency(h_rx, theta_r) * get_efficiency(h_ry, theta_r);
		efficiency_tree->Fill();
	}

	events_new->AddFriend("efficiency");
	return efficiency_tree;
}
