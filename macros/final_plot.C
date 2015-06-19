#include <stdio.h>
#include <set>

#include <TChain.h>
#include <TH1F.h>

#include "InelCrossSection.h"

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
TCanvas c_ratio("c_ratio", "ratio");
TCanvas c_combine("c_combine", "combine", 600, 600);
TH1F hcombine("combine", ";W [\\text{МэВ}];\\varkappa", InelCrossSection::BIN_COUNT, InelCrossSection::BIN_MIN, InelCrossSection::BIN_MAX);

void combine(const vector<TH1F*> &rs, TH1F *hcombine)
{
	Int_t xfirst  = hcombine->GetXaxis()->GetFirst();
	Int_t xlast   = hcombine->GetXaxis()->GetLast();
	for(Int_t bin = xfirst; bin <= xlast; bin++)
	{
		double sumw = 0;
		double sumxw = 0;
		for(auto it = rs.begin(); it != rs.end(); it++)
		{
			TH1F *r = *it;
			if (r->GetBinContent(bin) != 0)
			{
				double w = 1 / r->GetBinError(bin) / r->GetBinError(bin);
				sumw += w;
				sumxw += r->GetBinContent(bin) * w;
				cout << "[" << (long)r << "]" << r->GetBinContent(bin) << "\t";
			}
		}
		if (sumw != 0)
		{
			cout << sumxw / sumw << "\n";
			hcombine->SetBinContent(bin, sumxw / sumw);
			hcombine->SetBinError(bin, sqrt(1 / sumw));
		}
	}
}


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
	ymin = FLT_MAX;
	ymax = FLT_MIN;
	int color = 1;

	vector<TH1F*> rs;

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

		c.Process("InelCrossSection.C+");

		TH1F *r_r = (TH1F*)p->GetOutputList()->FindObject("fOutputRight");
		r_r->GetXaxis()->SetTitle("W [\\text{МэВ}]");
		r_r->GetYaxis()->SetTitle("\\sigma");
		r_r->SetLineColor(color);
		c_r.cd();
		r_r->Draw("same");


		TH1F *r_l = (TH1F*)p->GetOutputList()->FindObject("fOutputLeft");
		r_l->GetXaxis()->SetTitle("W [\\text{МэВ}]");
		r_l->GetYaxis()->SetTitle("\\sigma");
		r_l->SetLineColor(color);
		c_l.cd();
		r_l->Draw("same");

		static TH1F *r_r_first = r_r;
		static TH1F *r_l_first = r_l;

		//ymin = std::min(r_r->GetMinimum(0.), ymin);
		ymin = std::min(r_l->GetMinimum(0.), ymin);
		//ymax = std::max(r_r->GetMaximum(), ymax);
		ymax = std::max(r_l->GetMaximum(), ymax);
		r_l_first->GetYaxis()->SetRangeUser(0.98 * ymin, 1.02 * ymax);
		r_r_first->GetYaxis()->SetRangeUser(0.98 * ymin, 1.02 * ymax);
		c_l.Update();
		c_r.Update();

		TH1F *ratio = new TH1F("ratio", "", InelCrossSection::BIN_COUNT, InelCrossSection::BIN_MIN, InelCrossSection::BIN_MAX);
		ratio->GetXaxis()->SetTitle("W [\\text{МэВ}]");
		ratio->GetYaxis()->SetTitle("\\sigma_{\\text{right}} / \\sigma_{\\text{left}}");
		ratio->Divide(r_r, r_l);
		ratio->GetYaxis()->SetRangeUser(0.8, 1.2);
		ratio->SetLineColor(color);
		c_ratio.cd();
		ratio->Draw("same");
		c_ratio.Update();

		next_color(color);

		rs.push_back(r_l);
		c_combine.cd();
		combine(rs, &hcombine);
		hcombine.GetYaxis()->SetRangeUser(0.98 * ymin, 1.02 * ymax);
		hcombine.Draw();
		c_combine.Update();
	}
	fclose(fp);
}
