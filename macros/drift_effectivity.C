#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TFitResult.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TTree.h>

using std::string;

static TCanvas	c1, c2;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE2"), "READ");
static TTree	*events;
static const double	ANGLE_MAX = 2.0;
static const int	ANGLE_BINS = 25;
static const double	X_MIN = -200;
static const double	X_MAX = 120;
static const int	X_BINS = 50;

void	drift_effectivity()
{
	events = (TTree*)f.Get("events");

	TH2F *four_hit_theta_x = new TH2F(
		"four_hit_theta_x", "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	four_hit_theta_x->SetOption("zcol");
	TH2F *any_theta_x = new TH2F(
		"any_theta_x", "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	any_theta_x->SetOption("zcol");

	TH1F *four_hit_theta = new TH1F(
		"four_hit_theta", "",
		ANGLE_BINS, 0, ANGLE_MAX
		);
	TH1F *any_theta = new TH1F(
		"any_theta", "",
		ANGLE_BINS, 0, ANGLE_MAX
		);

	TH2F	*beam_profile = new TH2F("beam_profile", ";Z, [mm];Y, [mm]", 100, -20, 20, 100, -20, 20);
	beam_profile->SetOption("zcol");
	events->Draw("RL_y:RL_z >> beam_profile", "", "ZCOL");
	TF2	*xygaus = new TF2("xygaus", "xygaus");
	TFitResultPtr fit = beam_profile->Fit(xygaus, "S"); // S - return fit result 
	const double *params = fit->GetParams();
	const char	*cut = Form(
		"(abs(RL_z - (%f)) < %f) && "
		"(abs(RL_y - (%f)) < %f) && "
		"(RL_x < %f) && "
		"(RL_x > %f) && "
		"(abs(LR_z - (%f)) < %f) && "
		"(abs(LR_y - (%f)) < %f) && "
		"(LR_x < %f) && "
		"(LR_x > %f)",
		params[1], params[2] * 2,
		params[3], params[4] * 2,
		X_MAX, X_MIN,
		params[1], params[2] * 2,
		params[3], params[4] * 2,
		X_MAX, X_MIN
		);

	c1.cd();
	events->Draw("RL_x:theta_l >> any_theta_x", cut);
	events->Draw("RL_x:theta_l >> four_hit_theta_x", Form("(t3X_hits_count[0] == 4) && %s", cut));
	TH2F *u_tx = new TH2F(
		"effectivity_theta_x", "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	u_tx->Divide(four_hit_theta_x, any_theta_x);
	u_tx->GetXaxis()->SetTitle("\\Theta, rad");
	u_tx->GetYaxis()->SetTitle("X coordinate on target, mm");
	u_tx->SetOption("zcol");
	u_tx->Draw("zcol");

	c2.cd();
	events->Draw("theta_l >> any_theta", cut);
	events->Draw("theta_l >> four_hit_theta", Form("(t3X_hits_count[0] == 4) && %s", cut));
	TH1F *u_t = new TH1F(
		"effectivity_theta", "",
		ANGLE_BINS, 0, ANGLE_MAX
		);
	u_t->Divide(four_hit_theta, any_theta);
	u_t->GetXaxis()->SetTitle("\\Theta, rad");
	u_t->GetYaxis()->SetTitle("Effectivity, 1");
	u_t->Draw();

	c1.Show();
	c2.Show();
}
