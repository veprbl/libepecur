#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>

using std::string;

TCanvas	c1, c2;
TFile	f("26061082.root", "READ");
int	pad_id;

void	show_hist( TCanvas &c, string func )
{
	c.cd(++pad_id);
	TH2F *uu = new TH2F("uu", func.c_str(), 180, -90, 90, 380, 0, 380);
	events->Draw((func + " >> uu").c_str(), "", "col");
}

void	drift_calib()
{
	c1.Divide(2, 2);
	c1.Show();

	c2.Divide(2, 2);
	c2.Show();

	pad_id = 0;
	show_hist(c1, "d3X3_time:d3X3_wire_pos");
	show_hist(c1, "d4X3_time:d4X3_wire_pos");
	show_hist(c1, "d3X1_time:d3X1_wire_pos");
	show_hist(c1, "d4X1_time:d4X1_wire_pos");

	pad_id = 0;
	show_hist(c2, "d3Y3_time:d3Y3_wire_pos");
	show_hist(c2, "d4Y3_time:d4Y3_wire_pos");
	show_hist(c2, "d3Y1_time:d3Y1_wire_pos");
	show_hist(c2, "d4Y1_time:d4Y1_wire_pos");
}