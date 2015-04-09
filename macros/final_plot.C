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

		c.Process("InclCrossSection.C+");

		TH1F *r_r = (TH1F*)p->GetOutputList()->FindObject("fOutputRight");
		r_r->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		r_r->GetYaxis()->SetTitle("\\sigma");
		r_r->SetLineColor(color);
		r_r->GetYaxis()->SetRangeUser(ymin, ymax);
		c_r.cd();
		r_r->Draw("same");
		c_r.Update();


		TH1F *r_l = (TH1F*)p->GetOutputList()->FindObject("fOutputLeft");
		r_l->GetXaxis()->SetTitle("p_\\pi, [MeV/c]");
		r_l->GetYaxis()->SetTitle("\\sigma");
		r_l->SetLineColor(color);
		r_l->GetYaxis()->SetRangeUser(ymin, ymax);
		c_l.cd();
		r_l->Draw("same");
		c_l.Update();

		next_color(color);
	}
	fclose(fp);
}
