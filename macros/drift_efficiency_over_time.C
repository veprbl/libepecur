#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TTree.h>

using std::string;

static TCanvas	c;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*events;
static int	pad_id;
static int	max_time, min_time, time_bins;

static void	make_plot( const char* i )
{
	c.cd(++pad_id);

	char	*hist_name = Form("h%s_hit", i);
	TH1F	*hit = new TH1F(hist_name, "", time_bins, min_time, max_time);

	events->Draw(
		Form("timestamp >> %s", hist_name),
		Form("(t%s_hits_count == 4) && (t%s_track_count >= 1)", i, i)
		);

	hist_name = Form("h%s_all", i);
	TH1F	*all = new TH1F(hist_name, "", time_bins, min_time, max_time);

	events->Draw(
		Form("timestamp >> %s", hist_name)
		);

	hist_name = Form("h%s_eff", i);
	TH1F	*eff = new TH1F(hist_name, "", time_bins, min_time, max_time);
	eff->Divide(hit, all);
	eff->Draw();

	c.cd(++pad_id);
	hist_name = Form("h%s_eff_distrib", i);
	TH1F	*eff_distrib = new TH1F(hist_name, "", 100, 0, 0.5);
	int nbins = eff->GetXaxis()->GetNbins();
	for (int bin = 1; bin <= nbins; bin++) {
		eff_distrib->Fill(eff->GetBinContent(bin));
	}
	eff_distrib->Draw();
}

void	drift_efficiency_over_time()
{
	events = (TTree*)f.Get("events");
	events->SetEntryList((TEntryList*)f.Get("elist"));

	c.Divide(4, 2);

	max_time = events->GetMaximum("timestamp");
	min_time = events->GetMinimum("timestamp");
	time_bins = (max_time - min_time) / 60;

	make_plot("3X");
	make_plot("3Y");
	make_plot("4X");
	make_plot("4Y");

	c.Show();
}
