#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TTree.h>

using std::string;

static TCanvas	c;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*target_info_tree;
static int	pad_id;
static int      max_time, min_time, time_bins;

void	make_hist( const char *param, int index )
{
	char	*hist_name = Form("%s%i", param, index+1);
	new TH2F(hist_name, "", time_bins, min_time, max_time, 90, 0, 1000);
	c.cd(++pad_id);
	target_info_tree->Draw(Form("%s[%i]:time >> %s", param, index, hist_name));
}

void	target_info()
{
	target_info_tree = (TTree*)f.Get("target_info");

	max_time = target_info_tree->GetMaximum("time");
	min_time = target_info_tree->GetMinimum("time");
	time_bins = (max_time - min_time) / 60;

	c.Divide(8, 2);

	for(int i = 0; i < 7; i++)
	{
		make_hist("V", i);
	}
	c.cd(++pad_id);
	for(int i = 0; i < 8; i++)
	{
		make_hist("H", i);
	}

	c.Show();
}
