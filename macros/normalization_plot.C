#include <map>

#include <TChain.h>
#include <TH1F.h>

static TCanvas c1, c2;

void	normalization_plot()
{
	FILE *fp = fopen("../contrib/list_apr10",  "r");
	//FILE *fp = fopen("/tmp/list_apr10",  "r");

	char run_name[256];
	int momentum;
	int i = 0;
	while(fscanf(fp, "%s %i", run_name, &momentum) != EOF)
	{
		char *filename = Form("%s/%s.root", gSystem->Getenv("EPECUR_PROCESS_OUTDIR"), run_name);
		char *hist_name = Form("norm%i", i);
		i++;

		TDirectory *dir = gDirectory;
		TFile file(filename, "READ");
		TTree *t = file.Get("events");
		gDirectory = dir;
		if (!t)
		{
			Warning("", "%s - file not found. skipping.", filename);
			continue;
		}

		TH1F	*h = new TH1F(hist_name, hist_name, (1250-820)/1, 820, 1250);
		h->SetLineColor(i);
		h->GetYaxis()->SetRangeUser(0, 100000);

		long num_points = t->Draw(
			Form("beam_momentum >> %s", hist_name),
			"(event_cause & 0x2) == 0x2 && (abs(t1X_c0) < 90) && (abs(t1X_c0 + t1X_c1*540.0) < 90)"
			);
		cerr << filename << "\t" << num_points << endl;
	}

	c1.cd();
	int count = i;
	for(i = 0; i < count; i++)
	{
		TH1F *h = (TH1F*)gDirectory->Get(Form("norm%i", i));
		if (h)
		{
			h->Draw((i == 0) ? "" : "same");
		}
	}

	std::map<int, TH1F*>	hmap;
	std::vector<int>	momentums;
	fseek(fp, 0, SEEK_SET);
	i = 0;
	int color = 1;
	while(fscanf(fp, "%s %i", run_name, &momentum) != EOF)
	{
		if (!hmap.count(momentum))
		{
			char *mhist_name = Form("m%i", momentum);
			momentums.push_back(momentum);
			hmap[momentum] = new TH1F(mhist_name, mhist_name, (1250-820)/1, 820, 1250);
			hmap[momentum]->SetLineColor(color);
			hmap[momentum]->GetYaxis()->SetRangeUser(0, 300000);
			color++;
			if (color == 10) color = 20;
			cerr << "created " << mhist_name << endl;
		}
		TH1F *h = (TH1F*)gDirectory->Get(Form("norm%i", i));
cerr << "get " <<  h << endl;
		if (h)
		{
			hmap[momentum]->Add(h);
		}
		i++;
	}

	c2.cd();
	for(std::vector<int>::iterator it = momentums.begin();
		it != momentums.end(); ++it)
	{
		cerr << *it << endl;
		hmap[*it]->Draw((it == momentums.begin()) ? "" : "same");
	}

	fclose(fp);
}
