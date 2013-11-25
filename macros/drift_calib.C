#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>

using std::string;

TCanvas	c1, c2, c3;
TFile	f("26061082.root", "READ");
int	pad_id;
TTree	*events;

void	show_hist( TCanvas &c, string id )
{
	c.cd(++pad_id);
	string	func = id + "_time:" + id + "_wire_pos";
	TH2F *u = new TH2F(id.c_str(), func.c_str(), 100, -100, 100, 380, 0, 380);
	events->Draw((func + " >> " + id).c_str(), "", "zcol");
	u->SetStats(0);
}

void	drift_calib()
{
	events = (TTree*)f.Get("events");

	c1.Divide(2, 2);
	c1.Show();

	c2.Divide(2, 2);
	c2.Show();

	pad_id = 0;
	show_hist(c1, "d3X3");
	show_hist(c1, "d4X3");
	show_hist(c1, "d3X1");
	show_hist(c1, "d4X1");

	pad_id = 0;
	show_hist(c2, "d3Y3");
	show_hist(c2, "d4Y3");
	show_hist(c2, "d3Y1");
	show_hist(c2, "d4Y1");

	c3.cd();
	TH1F *u = new TH1F("d3X1_time_d", "", 380, 0, 380);
	u->GetXaxis()->SetTitle("T, [Counts]");
	u->GetYaxis()->SetTitle("N");
	events->Draw("d3X1_time >> d3X1_time_d");
	c3.Show();
}
