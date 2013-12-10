#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TString.h>
#include <TSystem.h>
#include <TText.h>
#include <TTree.h>

#include "utils.C"

using std::string;

static TCanvas	c1;
static TCanvas	c2;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*events;

TH1F*	makehist( string id, int ndf )
{
	int	hits_count = ndf + 2;
	TString	hist_name, cut, title;
	hist_name += id;
	hist_name += "_chisq_";
	hist_name += hits_count;
	hist_name += "hits";
	cut += id;
	cut += "_hits_count == ";
	cut += hits_count;
	float	min_x = 0;
	float	max_x = (ndf == 2) ? 2.0 : 0.5;
	TH1F	*hist = new TH1F(hist_name.Data(), cut, 1000, min_x, max_x);
	events->Draw(Form("%s_chisq >> %s", id.c_str(), hist_name.Data()), cut);
	if (id[1] == '3')
	{
		title += "Left ";
	}
	else
	{
		title += "Right ";
	}
	title += id[2];
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

	events = (TTree*)f.Get("events");

	c1.Divide(2, 2, 0.01, 0.02);
	c2.Divide(2, 2, 0.01, 0.02);

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

	TText	*t;
	TTree	*info = (TTree*)f.FindObjectAny("info");
	TString	info_str = get_info_str(info);
	c1.cd(0);
	t = new TText(0.005, 0.005, info_str);
	t->SetTextSize(0.02);
	t->Draw();
	c2.cd(0);
	t = new TText(0.005, 0.005, info_str);
	t->SetTextSize(0.02);
	t->Draw();

	c1.Show();
	c2.Show();
}
