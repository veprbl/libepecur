#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>

TCanvas	calib_curve_canvas;
TFile	f("26061082.root", "READ");

void	drift_calib_curve()
{
	TH1F	*calib_curve = 0, *copy = 0;

	drift_calib->SetBranchAddress("calib_curve", &calib_curve);

	calib_curve_canvas.Divide(4, 4);

	for(int i = 0; i < 16; i++)
	{
		drift_calib->GetEntry(i);

		calib_curve_canvas.cd(i + 1);
		TString	n("h");
		n += i;
		int	max_time = calib_curve->GetXaxis()->GetNbins();
		copy = new TH1F(n, n, max_time, 0.0, max_time);
		for(int j = 0; j <= max_time; j++)
		{
			copy->SetBinContent(j, calib_curve->GetBinContent(j) * 8.5);
		}
		copy->GetXaxis()->SetTitle("T, [Counts]");
		copy->GetYaxis()->SetTitle("X, [mm]");
		copy->SetStats(0);
		copy->Draw();

		calib_curve = 0; // TRICK :P
	}
}
