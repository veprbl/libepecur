#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TH1F.h>

TCanvas	angle_dist_canvas, calib_curve_canvas;
TFile	f("26061082.root", "READ");

void	drift_calib_curve()
{
	TH2F	*angle_dist = 0;
	TH1F	*calib_curve = 0;

	drift_calib->SetBranchAddress("angle_dist", &angle_dist);
	drift_calib->SetBranchAddress("calib_curve", &calib_curve);

	angle_dist_canvas.Divide(4, 4);
	calib_curve_canvas.Divide(4, 4);

	for(int i = 0; i < 16; i++)
	{
		drift_calib->GetEntry(i);

		angle_dist->SetLabelSize(0.08, "X");
		angle_dist->SetLabelSize(0.08, "Y");
		calib_curve->SetLabelSize(0.08, "X");

		angle_dist_canvas.cd(i + 1);
		angle_dist->Draw("col");
		calib_curve_canvas.cd(i + 1);
		calib_curve->Draw();

		angle_dist = 0; calib_curve = 0; // TRICK :P
	}

	angle_dist_canvas.Show();
	calib_curve_canvas.Show();
}
