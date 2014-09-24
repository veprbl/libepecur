#include <stdio.h>
#include <set>

#include <TChain.h>
#include <TH1F.h>

void	next_color(int &color)
{
	if (color < 9) color++;
	else
	switch(color)
	{
		case 9: color = 15; break;
		case 15: color = 20; break;
		case 20: color = 28; break;
		case 30: color = 35; break;
		case 35: color = 38; break;
		case 38: color = 46; break;
		default: color = 1;
	}
}

TCanvas c_l("c_l", "left"), c_r("c_r", "right");

void	final_plot()
{
	set<int> momentums;

	TProof *p = TProof::Open("");

	FILE *fp = fopen("../contrib/list_apr10",  "r");
	char run_name[256];
	int momentum;
	while(fscanf(fp, "%s %i", run_name, &momentum) != EOF)
	{
		momentums.insert(momentum);
	}

	double ymin, ymax;
	ymin = 0.0032;
	ymax = 0.0063;
	int color = 1;

	for(set<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
		fseek(fp, SEEK_SET, 0);

		TChain	c("events");
		c.SetProof();
		int run_momentum;
		while(fscanf(fp, "%s %i", run_name, &run_momentum) != EOF)
		{
			if (run_momentum != momentum) continue;

			cout << run_name << "\t" << momentum << endl;
			char *filename = Form("%s/%s.root", gSystem->Getenv("EPECUR_PROCESS_OUTDIR"), run_name);
			c.Add(filename);
		}
		Long64_t entries = c.GetEntries();
		cout << "Entries: " << entries << endl;

		char const *n_name = Form("n%i", momentum);
		char const *o_r_name = Form("o%i_r", momentum);
		char const *o_l_name = Form("o%i_l", momentum);

		TH1F	*n = new TH1F(n_name, "normalization", (1250-820), 820, 1250);
		c.Draw(Form("beam_momentum >> %s", n_name), "((event_cause & 0x2) == 0x2) && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25) && (min_cycle_efficiency > 0.15)"
			, "E1 GOFF", entries
			);
		TH1F	*o_r = new TH1F(o_r_name, "output right", (1250-820), 820, 1250);
		c.Draw(Form("beam_momentum >> %s", o_r_name),
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25) && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120) && (theta_r > 0) && (theta_l != theta_l) && (efficiency_r > 0.7) && (min_cycle_efficiency > 0.15) "/*&& (theta_r > 0.4) && (theta_r < 0.8) && (abs(phi_r+3.14/2)<0.6)*/") ? 1 : 0) / efficiency_r"
			, "E1 GOFF", entries
			);

		TH1F	*r_r = new TH1F(Form("r%i_r", momentum), "result_right", (1250-820), 820, 1250);
		r_r->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		r_r->GetYaxis()->SetTitle("\\sigma");
		r_r->SetLineColor(color);
		r_r->Sumw2();
		r_r->Divide(o_r, n, 1, 100);
		r_r->GetYaxis()->SetRangeUser(ymin, ymax);
		c_r.cd();
		r_r->Draw("same");
		c_r.Update();

		TH1F	*o_l = new TH1F(o_l_name, "output left", (1250-820), 820, 1250);
		c.Draw(Form("beam_momentum >> %s", o_l_name),
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25) && (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120) && (theta_l > 0) && (theta_r != theta_r) && (efficiency_l > 0.7) && (min_cycle_efficiency > 0.15) "/*&& (theta_l > 0.4) && (theta_l < 0.8) && (abs(phi_l-3.14/2)<0.6)*/") ? 1 : 0) / efficiency_l"
			, "E1 GOFF", entries
			);

		TH1F	*r_l = new TH1F(Form("r%i_l", momentum), "result_left", (1250-820), 820, 1250);
		r_l->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		r_l->GetYaxis()->SetTitle("\\sigma");
		r_l->SetLineColor(color);
		r_l->Sumw2();
		r_l->Divide(o_l, n, 1, 100);
		r_l->GetYaxis()->SetRangeUser(ymin, ymax);
		c_l.cd();
		r_l->Draw("same");
		c_l.Update();

		next_color(color);
	}
	fclose(fp);
}
