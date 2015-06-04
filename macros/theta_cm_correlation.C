#include <cmath>

#include <TCanvas.h>

#include "kinematics_common.h"

TCanvas c1("elastic_kinematics", "", 1200, 600), *c2;

/**
 * Calculate theta_cm from theta_pi in laboratory system
 */
double calc_theta_cm_from_theta_pi_lab(double beam_momentum, double theta_pi_lab)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);
	const double t = tan(theta_pi_lab);

	return
		acos(
			-gamma * abs(t) * beta
			/ p
			* sqrt(
				(sqr(M_pi) + sqr(p))
				/
				(1 + sqr(gamma * t))
			)
		)
		- atan(1 / (gamma * t));
}

/**
 * Calculate theta_cm from theta_p in laboratory system
 */
double calc_theta_cm_from_theta_p_lab(double beam_momentum, double theta_p_lab)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);
	const double t = tan(theta_p_lab);

	if (theta_p_lab > M_PI_2)
	{
		return NAN;
	}

	return
		acos(
			gamma * abs(t) * beta
			/ p
			* sqrt(
				(sqr(M_p) + sqr(p))
				/
				(1 + sqr(gamma * t))
			)
		)
		- atan(1 / (gamma * -t));
}

/**
 * Calculate theta_pi in laboratory system from theta_cm
 */
double calc_theta_lab_pi(double beam_momentum, double theta_cm)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);

	return
		atan2(
			p * sin(theta_cm)
			,
			gamma * (p * cos(theta_cm) + beta * sqrt(sqr(M_pi) + sqr(p)))
		);
}

/**
 * Calculate theta_p in laboratory ststem from theta_cm
 */
double calc_theta_lab_p(double beam_momentum, double theta_cm)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);

	return
		atan2(
			p * sin(theta_cm)
			,
			gamma * (-p * cos(theta_cm) + beta * sqrt(sqr(M_p) + sqr(p)))
		);
}

void assert_equal(double x1, double x2)
{
	if (fabs(x1 - x2) > 1e-3)
	{
		cout << x1 << "\t" << x2 << endl;
		throw "Assertion fault";
	}
}

void plot_kinematics()
{
	c1.Divide(2);
	const int N = 500;
	TGraph *g_pi_pi_theta_cm = new TGraph(N);
	TGraph *g_pi_p_lab = new TGraph(N);
	g_pi_pi_theta_cm->SetLineColor(kRed);
	g_pi_p_lab->SetLineColor(kBlue);
	TGraph *g_p_pi_theta_cm = new TGraph(N / 2);
	TGraph *g_p_pi_lab = new TGraph(N / 2);
	g_p_pi_theta_cm->SetLineColor(kRed);
	g_p_pi_lab->SetLineColor(kGreen);
	for(int i = 0; i < N; i++)
	{
		const double theta_lab = M_PI / N * i;
		const double beam_momentum = 1234.5; // arbitrary

		{
			assert_equal(theta_lab, calc_theta_lab_pi(beam_momentum, calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_lab)));

			double theta_cm = calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_lab);
			double theta_p_lab = calc_theta_lab_p(beam_momentum, theta_cm);

			g_pi_pi_theta_cm->SetPoint(i, theta_lab * TMath::RadToDeg(), theta_cm * TMath::RadToDeg());
			g_pi_p_lab->SetPoint(i, theta_lab * TMath::RadToDeg(), theta_p_lab * TMath::RadToDeg());
		}

		// protons can't fly back
		if (theta_lab < M_PI_2)
		{
			assert_equal(theta_lab, calc_theta_lab_p(beam_momentum, calc_theta_cm_from_theta_p_lab(beam_momentum, theta_lab)));

			double theta_cm = calc_theta_cm_from_theta_p_lab(beam_momentum, theta_lab);
			double theta_pi_lab = calc_theta_lab_pi(beam_momentum, theta_cm);

			g_p_pi_theta_cm->SetPoint(i, theta_lab * TMath::RadToDeg(), theta_cm * TMath::RadToDeg());
			g_p_pi_lab->SetPoint(i, theta_lab * TMath::RadToDeg(), theta_pi_lab * TMath::RadToDeg());
		}
	}
	g_pi_p_lab->RemovePoint(0);
	TLegend *legend;
	c1.cd(1);
	g_pi_pi_theta_cm->GetXaxis()->SetLimits(0.0, 180.0);;
	g_pi_pi_theta_cm->GetHistogram()->SetMaximum(180.0);
	g_pi_pi_theta_cm->Draw("AL");
	g_pi_p_lab->Draw("SAME");
	g_pi_pi_theta_cm->GetXaxis()->SetTitle("\\Theta_{\\text{лаб}}\\text{ пиона}");
	legend = new TLegend(0.15,0.7,0.45,0.9);
	legend->AddEntry(g_pi_pi_theta_cm, "\\Theta_{\\text{ц.м.}}\\text{ пиона}", "l");
	legend->AddEntry(g_pi_p_lab, "\\Theta_{\\text{лаб}}\\text{ протона}", "l");
	legend->Draw();
	c1.cd(2);
	g_p_pi_theta_cm->GetXaxis()->SetLimits(0.0, 180.0);;
	g_p_pi_theta_cm->GetHistogram()->SetMaximum(180.0);
	g_p_pi_theta_cm->Draw("AL");
	g_p_pi_lab->Draw("SAME");
	g_p_pi_theta_cm->GetXaxis()->SetTitle("\\Theta_{\\text{лаб}}\\text{ протона}");
	legend = new TLegend(0.6-0.05,0.7,0.9-0.05,0.9);
	legend->AddEntry(g_p_pi_theta_cm, "\\Theta_{\\text{ц.м.}}\\text{ пиона}", "l");
	legend->AddEntry(g_p_pi_lab, "\\Theta_{\\text{лаб}}\\text{ пиона}", "l");
	legend->Draw();
	c1.SaveAs("elastic_kinematics.tex");
}

void plot_correlation()
{
#if 0
	TChain *events = new TChain("events");
        events->Add(Form("%s/*.root", gSystem->Getenv("EPECUR_PROCESS_OUTDIR")));
	const char *run_name = "all runs";
#else
	TTree *events = (TTree*)_file0->Get("events");
	TObjArray *subStrL = TPRegexp("(\\d+).root$").MatchS(_file0->GetName());
	TString run_name_str = ((TObjString *) subStrL->At(1))->GetString();
	const char *run_name = run_name_str.Data();
#endif

	double beam_momentum, theta_l, theta_r;
	events->SetBranchAddress("beam_momentum", &beam_momentum);
	events->SetBranchAddress("theta_l", &theta_l);
	events->SetBranchAddress("theta_r", &theta_r);

	TH2F *h1 = new TH2F("h1", run_name, 50, -30, 30, 50, 0.0, 180.0);
	h1->SetXTitle("\\Theta_{\\text{ц.м.;p;право}} - \\Theta_{\\text{ц.м.;}\\pi\\text{;лево}}");
	h1->SetYTitle("\\Theta_{\\text{ц.м.;}\\pi\\text{;лево}}");
	h1->SetStats(kFALSE);
	// right <-> left exchange
	TH2F *h2 = new TH2F("h2", run_name, 50, -30, 30, 50, 0.0, 180.0);
	h2->SetXTitle("\\Theta_{cm,p,left} - \\Theta_{cm,pi,right}");
	h2->SetYTitle("\\Theta_{cm,pi,right}");
	h2->SetStats(kFALSE);

	int count = events->GetEntries();
	for(int i = 0; i < count; i++)
	{
		events->GetEntry(i);
		if (std::isnan(theta_l) || std::isnan(theta_r)) continue;
		double theta_cm_pi_left = calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_l);
		double theta_cm_p_right = calc_theta_cm_from_theta_p_lab(beam_momentum, theta_r);
		h1->Fill((theta_cm_p_right - theta_cm_pi_left) * TMath::RadToDeg(), theta_cm_pi_left * TMath::RadToDeg());
		double theta_cm_pi_right = calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_r);
		double theta_cm_p_left = calc_theta_cm_from_theta_p_lab(beam_momentum, theta_l);
		h2->Fill((theta_cm_p_left - theta_cm_pi_right) * TMath::RadToDeg(), theta_cm_pi_right * TMath::RadToDeg());
	}
	c2 = new TCanvas(Form("c%s_thetacm_correlation", run_name), "", 1200, 600);
	c2->Divide(2);
	c2->cd(1); h1->Draw("ZCOL");
	c2->cd(1)->SaveAs("elastic_kinematics_distribution.tex");
	c2->cd(2); h2->Draw("ZCOL");
}

void theta_cm_correlation()
{
	plot_kinematics();
	plot_correlation();
}
