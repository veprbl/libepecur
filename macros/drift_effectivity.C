#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>

using std::string;

TCanvas	c1;
TFile	f("26061082-pass2.root", "READ");
int	pad_id = 0;
const double	ANGLE_MAX = 2.0;
const int	ANGLE_BINS = 25;
const double	X_MIN = -200;
const double	X_MAX = 120;

void	drift_effectivity()
{
	TH1F *four_hit = new TH1F("four_hit", "", ANGLE_BINS, 0, ANGLE_MAX);
	TH1F *any = new TH1F("any", "", ANGLE_BINS, 0, ANGLE_MAX);

	beam_profile = new TH2F("beam_profile", ";Z, [mm];Y, [mm]", 100, -20, 20, 100, -20, 20);
	events->Draw("RL_y:RL_z >> beam_profile", "", "ZCOL");
	xygaus = new TF2("xygaus", "xygaus");
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

	events->Draw("theta_l >> any", cut);
	events->Draw("theta_l >> four_hit", Form("(t3X_hits_count[0] == 4) && %s", cut));
	TH1F *u = new TH1F((*four_hit) / (*any));
	u->GetXaxis()->SetTitle("\\Theta, rad");
	u->GetYaxis()->SetTitle("Effectivity, 1");
	u->Draw();
	c1.Show();
}
