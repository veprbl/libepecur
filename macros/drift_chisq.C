#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1F.h>
#include <TString.h>
#include <TSystem.h>
#include <TText.h>
#include <TTree.h>
#include <TStyle.h>

#include "utils.C"

using std::string;

static TCanvas	c1("c1", "", 600, 600);
static TCanvas	c2("c2", "", 600, 600);
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
	float	max_x = (ndf == 2) ? 2.5 : 0.5;
	TH1F	*hist = new TH1F(hist_name.Data(), "", 1000, min_x, max_x);
	events->Draw(Form("%s_chisq[0] >> %s", id.c_str(), hist_name.Data()), cut);
	if (hits_count > 3)
	{
		TF1 *func = new TF1("func", "[0]*exp([1]*x)+[2]");
		TFitResultPtr fit = hist->Fit(func, "SQ", "", 0.0, 0.5); // S - return fit result, Q - quiet
		std::cout << hist_name << "\tp1: " << fit->Parameter(1) << "\tsigma: " << sqrt(-1.0/2/fit->Parameter(1)) << "\tadd const: " << fit->Parameter(2) << std::endl;
	}
	hist->GetXaxis()->SetTitle("\\chi^2\\text{, мм}^2");
	gPad->SaveAs(hist_name + ".tex");

	return hist;
}

void	drift_chisq()
{
	int	i;

	gStyle->SetPaperSize(8,8);

	events = (TTree*)f.Get("events");

	i = 0;
	c1.cd();
	makehist("t3X", 2);
	makehist("t4X", 2);
	makehist("t3Y", 2);
	makehist("t4Y", 2);

	c1.Show();
	c2.Show();
}
