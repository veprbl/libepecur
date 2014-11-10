#include <stdio.h>
#include <set>
#include <float.h>

#include <TChain.h>
#include <TH1F.h>
#include <TMath.h>

TCanvas c1;

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
double ymin = FLT_MAX, ymax = FLT_MIN;
int color = 1;
	for(set<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
//momentum=1060;
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
		cout << entries << endl;

		TH1F	*or = new TH1F("or", "output", (1250-820), 820, 1250);
		c.Draw("beam_momentum >> or",
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25) && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120) && (theta_r > 0) && (theta_l != theta_l) && (efficiency_r > 0.7)) ? 1 : 0) / efficiency_r"
			, "E1", entries
			);
		TH1F	*ol = new TH1F("ol", "output", (1250-820), 820, 1250);
		c.Draw("beam_momentum >> ol",
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25) && (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120) && (theta_l > 0) && (theta_r != theta_r) && (efficiency_l > 0.7)) ? 1 : 0) / efficiency_l"
			, "E1", entries
			);
/*		TH1F    *o = new TH1F("o", "output", (1250-820), 820, 1250);
		o->Add(or);
		o->Add(ol);
		o->SetName("o");*/
		TH1F	*n = new TH1F("n", "normalization", (1250-820), 820, 1250);
		c.Draw("beam_momentum >> n", "((event_cause & 0x2) == 0x2)  && (abs(t1X_c0+t1X_c1*540)<80) && ((t2X_c0+t2X_c1*990)**2 + (t2Y_c0+t2Y_c1*990)**2<25)"
			, "E1", entries
			);

		double rval = or->GetBinContent(or->FindBin(run_momentum));
		double lval = ol->GetBinContent(ol->FindBin(run_momentum));
		cout << rval << "\t" << lval << endl;

/*		ymin = TMath::Min(TMath::Min(ymin, lval), rval);
		ymax = TMath::Max(TMath::Max(ymax, lval), rval);*/
		ymin = 0.0032;
		ymax = 0.0063;

		TH1F	*rr = new TH1F(Form("r%i_r", momentum), "result_right", (1250-820), 820, 1250);
		rr->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		rr->GetYaxis()->SetTitle("\\sigma");
		rr->SetLineColor(color);
		rr->Sumw2();
		rr->Divide(or, n, 1, 100);

		TH1F	*rl = new TH1F(Form("r%i_l", momentum), "result_left", (1250-820), 820, 1250);
		rl->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		rl->GetYaxis()->SetTitle("\\sigma");
		rl->SetLineColor(color);
		rl->Sumw2();
		rl->Divide(ol, n, 1, 100);
		delete or;
		delete ol;
		//delete o;
		delete n;
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
//break;
	}
	fclose(fp);

	TCanvas *cr = new TCanvas();
	for(set<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
		TH1F *h = (TH1F*)gDirectory->Get(Form("r%i_r", momentum));
		if (h)
		{
			h->GetYaxis()->SetRangeUser(ymin, ymax);
			h->Draw("same");
		}
	}
	TCanvas *cl = new TCanvas();
	for(set<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
		TH1F *h = (TH1F*)gDirectory->Get(Form("r%i_l", momentum));
		if (h)
		{
			h->GetYaxis()->SetRangeUser(ymin, ymax);
			h->Draw("same");
		}
	}
}
