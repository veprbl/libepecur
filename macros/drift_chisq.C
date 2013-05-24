#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TString.h>

using std::string;

TCanvas	c1;
TCanvas	c2;
TFile	f("26061082.root", "READ");

TH1F*	makehist( string name, int ndf )
{
	int	hits_count = ndf + 2;
	TString	cut, title;
	cut += name;
	cut += "_hits_count == ";
	cut += hits_count;
	float	min_x = 0;
	float	max_x = (ndf == 2) ? 2.0 : 0.5;
	TH1F	*hist = new TH1F(name.c_str(), cut, 1000, min_x, max_x);
	events->Draw((name + "_chisq >> " + name).c_str(), cut);
	if (name[1] == '3')
	{
		title += "Left ";
	}
	else
	{
		title += "Right ";
	}
	title += name[2];
	title += " (";
	title += cut;
	title += ")";
	hist->SetTitle(title);
	hist->SetLabelSize(0.04, "X");
	hist->SetLabelSize(0.04, "Y");
	hist->SetTitleSize(0.05, "X");
	hist->SetTitleSize(0.05, "Y");
	hist->SetTitleOffset(0.9, "X");
	hist->SetTitleOffset(1.1, "Y");
	hist->GetXaxis()->SetTitle("#chi^2, [mm]");
	hist->GetYaxis()->SetTitle("N");

	return hist;
}

void	drift_chisq()
{
	int	i;

	c1.Divide(2, 2, 0.01, 0.01);
	c2.Divide(2, 2, 0.01, 0.01);

	i = 0;
	c1.cd(++i);
	makehist("t3X", 2);
	c1.cd(++i);
	makehist("t4X", 2);
	c1.cd(++i);
	makehist("t3Y", 2);
	c1.cd(++i);
	makehist("t4Y", 2);

	i = 0;
	c2.cd(++i);
	makehist("t3X", 1);
	c2.cd(++i);
	makehist("t4X", 1);
	c2.cd(++i);
	makehist("t3Y", 1);
	c2.cd(++i);
	makehist("t4Y", 1);

	c1.Show();
	c2.Show();
}
