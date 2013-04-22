#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TString.h>

using std::string;

TCanvas	c1;
TFile	f("26061082.root", "READ");

TH1F*	makehist( string name, int ndf )
{
	int	hits_count = ndf + 2;
	TString	cut;
	cut += name;
	cut += "_hits_count == ";
	cut += hits_count;
	TH1F	*hist = new TH1F(name.c_str(), cut, 1000, 0, 2);
	events->Draw((name + "_chisq >> " + name).c_str(), cut);
	hist->SetLabelSize(0.05, "X");
	hist->SetLabelSize(0.05, "Y");

	return hist;
}

void	drift_chisq()
{
	int	i;

	c1.Divide(4, 2);

	c1.cd(++i);
	makehist("t3X", 2);
	c1.cd(++i);
	makehist("t4X", 2);
	c1.cd(++i);
	makehist("t3Y", 2);
	c1.cd(++i);
	makehist("t4Y", 2);

	c1.cd(++i);
	makehist("t3X", 1);
	c1.cd(++i);
	makehist("t4X", 1);
	c1.cd(++i);
	makehist("t3Y", 1);
	c1.cd(++i);
	makehist("t4Y", 1);

	c1.Show();
}
