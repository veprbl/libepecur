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

static TCanvas	c1, c2, c3;
static const double	ANGLE_MAX = 2.0 * TMath::RadToDeg();
static const int	ANGLE_BINS = 25;
static const double	X_MIN = -200;
static const double	X_MAX = 120;
static const int	X_BINS = 50;

TH1F*	make_drift_efficiency_hist(char arm, char axis, TTree *events)
{
	if ((arm != 'l') && (arm != 'r'))
	{
		throw "make_drift_efficiency_hist: Invalid arm value";
	}
	if ((axis != 'X') && (axis != 'Y'))
	{
		throw "make_drift_efficiency_hist: Invalid axis value";
	}
	char arm_chamber = (arm == 'l') ? '3' : '4';
	int canvas_cell = 1 + (int)(arm == 'l') + 2 * (int)(axis == 'X');

	TH2F *four_hit_theta_x = new TH2F(
		Form("%c%c_four_hit_theta_x", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	four_hit_theta_x->SetOption("zcol");
	TH2F *any_theta_x = new TH2F(
		Form("%c%c_any_theta_x", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	any_theta_x->SetOption("zcol");

	TH1F *four_hit_theta = new TH1F(
		Form("%c%c_four_hit_theta", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX
		);
	TH1F *any_theta = new TH1F(
		Form("%c%c_any_theta", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX
		);

	c1.cd();
	TH2F	*beam_profile = new TH2F("beam_profile", ";Z, [mm];Y, [mm]", 100, -20, 20, 100, -20, 20);
	beam_profile->SetOption("zcol");
	events->Draw("RL_y:RL_z >> beam_profile", "", "ZCOL");
	TF2	*xygaus = new TF2("xygaus", "xygaus");
	TFitResultPtr fit = beam_profile->Fit(xygaus, "S"); // S - return fit result 
	const double *params = fit->GetParams();
	if (!params)
	{
		throw "Error determining beam profile parameters.";
	}
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

	c2.cd(canvas_cell);
	events->Draw(
		Form("RL_x:theta_%c * TMath::RadToDeg() >> %s", arm, any_theta_x->GetName()),
		cut
		);
	events->Draw(
		Form("RL_x:theta_%c * TMath::RadToDeg() >> %s", arm, four_hit_theta_x->GetName()),
		Form("(t%c%c_hits_count[0] == 4) && %s", arm_chamber, axis, cut)
		);
	TH2F *u_tx = new TH2F(
		Form("%c%c_efficiency_theta_x", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX,
		X_BINS, X_MIN, X_MAX
		);
	u_tx->Divide(four_hit_theta_x, any_theta_x);
	u_tx->GetXaxis()->SetTitle("\\Theta");
	u_tx->GetYaxis()->SetTitle("X coordinate on target, mm");
	u_tx->SetOption("zcol");
	u_tx->Draw("zcol");

	c3.cd(canvas_cell);
	events->Draw(
		Form("theta_%c * TMath::RadToDeg() >> %s", arm, any_theta->GetName()),
		cut
		);
	events->Draw(
		Form("theta_%c * TMath::RadToDeg() >> %s", arm, four_hit_theta->GetName()),
		Form("(t%c%c_hits_count[0] == 4) && %s", arm_chamber, axis, cut)
		);
	TH1F *u_t = new TH1F(
		Form("%c%c_efficiency_theta", arm, axis), "",
		ANGLE_BINS, 0, ANGLE_MAX
		);
	u_t->Divide(four_hit_theta, any_theta);
	u_t->GetXaxis()->SetTitle("\\Theta");
	u_t->GetYaxis()->SetTitle("Efficiency, 1");
	u_t->GetYaxis()->SetRangeUser(0.0, 1.0);
	u_t->Draw();

	return u_t;
}

void	drift_efficiency()
{
	TDirectory	*rootdir = gDirectory;
	TFile	f(gSystem->Getenv("EPECUR_ROOTFILE2"), "READ");
	TTree	*events = (TTree*)f.Get("events");
	rootdir->cd();

	c2.Divide(2, 2);
	c3.Divide(2, 2);

	rootdir->mkdir("lx")->cd();
	make_drift_efficiency_hist('l', 'X', events);
	rootdir->mkdir("ly")->cd();
	make_drift_efficiency_hist('l', 'Y', events);
	rootdir->mkdir("rx")->cd();
	make_drift_efficiency_hist('r', 'X', events);
	rootdir->mkdir("ry")->cd();
	make_drift_efficiency_hist('r', 'Y', events);

	c1.Show();
	c2.Show();
	c3.Show();
}
