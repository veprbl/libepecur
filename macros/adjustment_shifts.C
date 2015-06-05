#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>

static TCanvas	c;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE2"), "READ");
static int		pad_id = 0;
static TTree	*events;

/*!
 * This script is showing coordinate difference distribution for
 * closest points on two tracks that 'process' utility intersects.
 * This value should be zero-centered. If it's not it basically
 * means that one block of chambers is shifted relative to the
 * other.
 */
void	adjustment_shifts()
{
	events = (TTree*)f.Get("events");

	c.Divide(3, 3);
	c.Show();

	c.cd(++pad_id);
	events->Draw("(RL_x-LR_x) >> rl_x", "abs(RL_x - LR_x) < 1");
	((TH1*)gDirectory->Get("rl_x"))->GetXaxis()->SetTitle("\\Delta \\text{RL}_x");
	c.cd(++pad_id);
	events->Draw("(RL_y-LR_y) >> rl_y", "abs(RL_y - LR_y) < 2");
	((TH1*)gDirectory->Get("rl_y"))->GetXaxis()->SetTitle("\\Delta \\text{RL}_y");
	c.cd(++pad_id);
	events->Draw("(RL_z-LR_z) >> rl_z", "abs(RL_z - LR_z) < 30");
	((TH1*)gDirectory->Get("rl_z"))->GetXaxis()->SetTitle("\\Delta \\text{RL}_z");

	c.cd(++pad_id);
	events->Draw("(LF2_x-F2L_x) >> lf2_x", "abs(LF2_x - F2L_x) < 1");
	((TH1*)gDirectory->Get("lf2_x"))->GetXaxis()->SetTitle("\\Delta \\text{LF2}_x");
	c.cd(++pad_id);
	events->Draw("(LF2_y-F2L_y) >> lf2_y", "abs(LF2_y - F2L_y) < 2");
	((TH1*)gDirectory->Get("lf2_y"))->GetXaxis()->SetTitle("\\Delta \\text{LF2}_y");
	c.cd(++pad_id);
	events->Draw("(LF2_z-F2L_z) >> lf2_z", "abs(LF2_z - F2L_z) < 30");
	((TH1*)gDirectory->Get("lf2_z"))->GetXaxis()->SetTitle("\\Delta \\text{LF2}_z");

	c.cd(++pad_id);
	events->Draw("(F2R_x-RF2_x) >> f2r_x", "abs(F2R_x - RF2_x) < 1");
	((TH1*)gDirectory->Get("f2r_x"))->GetXaxis()->SetTitle("\\Delta \\text{F2R}_x");
	c.cd(++pad_id);
	events->Draw("(F2R_y-RF2_y) >> f2r_y", "abs(F2R_y - RF2_y) < 2");
	((TH1*)gDirectory->Get("f2r_y"))->GetXaxis()->SetTitle("\\Delta \\text{F2R}_y");
	c.cd(++pad_id);
	events->Draw("(F2R_z-RF2_z) >> f2r_z", "abs(F2R_z - RF2_z) < 30");
	((TH1*)gDirectory->Get("f2r_z"))->GetXaxis()->SetTitle("\\Delta \\text{F2R}_z");
}
