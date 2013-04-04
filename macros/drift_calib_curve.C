#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TH1F.h>

TCanvas	calib_curve_canvas;
TFile	f("26061082.root", "READ");

void	drift_calib_curve()
{
	TH1F	*calib_curve = 0;

	drift_calib->SetBranchAddress("calib_curve", &calib_curve);

	calib_curve_canvas.Divide(4, 4);

	for(int i = 0; i < 16; i++)
	{
		drift_calib->GetEntry(i);

		calib_curve->SetLabelSize(0.08, "X");

		calib_curve_canvas.cd(i + 1);
		calib_curve->Draw();

		calib_curve = 0; // TRICK :P
	}

	calib_curve_canvas.Show();
}
