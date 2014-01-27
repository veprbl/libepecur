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

	TH1F	*h = make_drift_efficiency_hist(events_new);

	efficiency_tree->Branch("efficiency_l", NULL, "efficiency_l/D")->SetAddress(&efficiency_l);
	efficiency_tree->Branch("efficiency_r", NULL, "efficiency_r/D")->SetAddress(&efficiency_r);

	events_new->GetBranch("theta_l")->SetAddress(&theta_l);
	events_new->GetBranch("theta_r")->SetAddress(&theta_r);

	std::cerr << events_new->GetEntries() << std::endl;

	for(int i = 0; i < events_new->GetEntries(); i++)
	{
		events_new->GetEntry(i);

		efficiency_l = get_efficiency(h, theta_l);
		efficiency_r = get_efficiency(h, theta_r);
		efficiency_tree->Fill();
	}

    events_new->AddFriend("efficiency");
	return efficiency_tree;
}
