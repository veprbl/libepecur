#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>

using std::string;

TCanvas	c1;
TFile	f("26061082.root", "READ");

TH1F*	makehist( string name )
{
	TH1F	*hist = new TH1F(name.c_str(), name.c_str(), 1000, 0, 2);
	events->Draw((name + " >> " + name).c_str()), (name + " < 20").c_str());
	hist->SetLabelSize(0.05, "X");
	hist->SetLabelSize(0.05, "Y");

	return hist;
}

void	drift_chisq()
{
	int	i;

	c1.Divide(2, 2);

	c1.cd(++i);
	makehist("t3X_chisq");
	c1.cd(++i);
	makehist("t4X_chisq");

	c1.cd(++i);
	makehist("t3Y_chisq");
	c1.cd(++i);
	makehist("t4Y_chisq");

	c1.Show();
}
