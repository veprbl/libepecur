#include <cmath>

#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TMath.h>
#include <TSystem.h>
#include <TTree.h>

static const int N = 5;
static TCanvas	c("c", "", 1600.0, 1600.0 / N);
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*events;

void	mk_plot(int i, bool time_domain, double angle, double angle_bin_width)
{
	c.cd(i);
	char *hname = Form("h%i", i);
	double bin_count, bin_min, bin_max;
	const char *correlation;
	if (time_domain)
	{
		bin_count = 380;
		bin_min = 0.0;
		bin_max = 380;
		correlation = "d3Y1_time[0]:d3Y2_time[0]";
	}
	else
	{
		bin_count = 100;
		bin_min = 0.0;
		bin_max = 8.5;
		correlation = "d3Y1_drift_offset[0]:d3Y2_drift_offset[0]";
	}
	TH2F *h = new TH2F(hname, Form("angle = %.0f \\pm %.0f", angle, angle_bin_width / 2), bin_count, bin_min, bin_max, bin_count, bin_min, bin_max);
	if (time_domain)
	{
		h->SetXTitle("d3Y2_time, counts");
		h->SetYTitle("d3Y1_time, counts");
	}
	else
	{
		h->SetXTitle("calibration_curve(d3Y2_time), mm");
		h->SetYTitle("calibration_curve(d3Y1_time), mm");
	}
	events->Draw(
		Form("%s >> %s", correlation, hname),
		Form(
			"(d3Y1_num_wires == 1) && (d3Y2_num_wires == 1)"
			" && (abs(abs(d3Y1_wire_pos[0] - d3Y2_wire_pos[0]) - 1.0) < 0.0000000001)"
			" && (t3Y_track_count == 1)"
			" && (abs(abs(atan(t3Y_c1[0])) - %f) < %f)", angle * TMath::DegToRad(), angle_bin_width / 2 * TMath::DegToRad()),
		"COL Z"
		);

}

void	drift_sum_rule()
{
	events = (TTree*)f.Get("events");

	c.Divide(N);

	double angle = 0.0;
	const double STEP = 30.0/(N-1);
	for(int i = 1; i <= N; i++)
	{
		mk_plot(i, false, angle, 0.5 * STEP);
		angle += STEP;
	}
}
