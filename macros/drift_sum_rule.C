#include <cmath>

#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TTree.h>

static const int N = 5;
static TCanvas	c("c", "", 1600.0, 1600.0 / N);
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*events;

void	mk_plot(int i, double angle)
{
	c.cd(i);
	char *hname = Form("h%i", i);
	TH2F *h = new TH2F(hname, Form("angle = %.0f \\pm 5", angle), 100, 0.0, 8.5, 100, 0.0, 8.5);
	h->SetXTitle("calibration_curve(d3Y2_time), mm");
	h->SetYTitle("calibration_curve(d3Y1_time), mm");
	const double F3_Y1Y2_distance = 100.0 - (-222.5); // from geom_apr10.c
	events->Draw(
		Form("d3Y1_drift_offset[0]:d3Y2_drift_offset[0] >> %s", hname),
		Form(
			"(d3Y1_num_wires == 1) && (d3Y2_num_wires == 1)"
			" && (abs(abs(d3Y1_wire_pos[0] - d3Y2_wire_pos[0]) - 1.0) < 0.0000000001)"
			" && (d3Y3_num_wires == 1)"
			" && (abs(abs(d3Y1_wire_pos[0] - d3Y3_wire_pos[0]) - %f) < 1.0)", std::tan(angle*3.14/180) * F3_Y1Y2_distance / 8.5),
		"COL Z"
		);

}

void	drift_sum_rule()
{
	events = (TTree*)f.Get("events");

	c.Divide(N);

	double angle = 0.0;
	for(int i = 1; i <= N; i++)
	{
		mk_plot(i, angle);
		angle += 30.0/(N-1);
	}
}
