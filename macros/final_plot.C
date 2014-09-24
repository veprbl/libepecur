#include <stdio.h>

#include <TChain.h>
#include <TH1F.h>

TCanvas c1;

void	final_plot()
{
	vector<int> momentums; // momentum -> [run_id]

	FILE *fp = fopen("../contrib/list_apr10",  "r");
	char run_name[256];
	int momentum;
	while(fscanf(fp, "%s %i", run_name, &momentum) != EOF)
	{
		bool unique = true;
		for(vector<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
		{
			if ((*it) == momentum)
			{
				unique = false;
				continue;
			}
		}
		if (unique)
		{
			momentums.push_back(momentum);
		}
	}
int color = 1;
	for(vector<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
		fseek(fp, SEEK_SET, 0);

		TChain	c("events");
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
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120) && (theta_r > 0) && (theta_l != theta_l) && (efficiency_r > 0.7)) ? 1 : 0) / efficiency_r"
			, "E1", entries
			);
		TH1F	*ol = new TH1F("ol", "output", (1250-820), 820, 1250);
		c.Draw("beam_momentum >> ol",
	"((((event_cause & 0x1) == 0x1) && (abs(t1X_c0+t1X_c1*540)<80) && (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120) && (theta_l > 0) && (theta_r != theta_r) && (efficiency_l > 0.7)) ? 1 : 0) / efficiency_l"
			, "E1", entries
			);
		TH1F	*o = new TH1F(*or + *ol);
		o->SetName("o");
		TH1F	*n = new TH1F("n", "normalization", (1250-820), 820, 1250);
		c.Draw("beam_momentum >> n", "((event_cause & 0x2) == 0x2)  && (abs(t1X_c0+t1X_c1*540)<80)"
			, "E1", entries
			);
		TH1F	*r = new TH1F(Form("r%i", momentum), "result", (1250-820), 820, 1250);
		r->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		r->GetYaxis()->SetTitle("\\sigma");
		r->SetLineColor(color);
		r->Sumw2();
		r->Divide(o, n, 1, 100);
		delete or; delete ol; delete o; delete n;
//if (i>=4)break;
color++;
if (color == 10) color = 20;
	}
	fclose(fp);

	for(vector<int>::iterator it = momentums.begin(); it != momentums.end(); it++)
	{
		int momentum = *it;
		TH1F *h = (TH1F*)gDirectory->Get(Form("r%i", momentum));
cout << momentum << "\t" << h << endl;
		if (h)
		{
			h->Draw("same");
		}
	}
}
