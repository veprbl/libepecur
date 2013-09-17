#include <TFile.h>
#include <TCanvas.h>

TCanvas	c;
TFile	f("26061082.root", "READ");
int		pad_id = 0;

/*!
 * This script is showing coordinate difference distribution for
 * closest points on two tracks that 'process' utility intersects.
 * This value should be zero-centered. If it's not it basically
 * means that one block of chambers is shifted relative to the
 * other.
 */
void	adjustment_shifts()
{
	c.Divide(3, 3);
	c.Show();

	c.cd(++pad_id);
	intersections->Draw("(RL_x-LR_x)", "abs(RL_x - LR_x) < 1");
	c.cd(++pad_id);
	intersections->Draw("(RL_y-LR_y)", "abs(RL_y - LR_y) < 2");
	c.cd(++pad_id);
	intersections->Draw("(RL_z-LR_z)", "abs(RL_z - LR_z) < 30");

	c.cd(++pad_id);
	intersections->Draw("(LF2_x-F2L_x)", "abs(LF2_x - F2L_x) < 1");
	c.cd(++pad_id);
	intersections->Draw("(LF2_y-F2L_y)", "abs(LF2_y - F2L_y) < 2");
	c.cd(++pad_id);
	intersections->Draw("(LF2_z-F2L_z)", "abs(LF2_z - F2L_z) < 30");

	c.cd(++pad_id);
	intersections->Draw("(F2R_x-RF2_x)", "abs(F2R_x - RF2_x) < 1");
	c.cd(++pad_id);
	intersections->Draw("(F2R_y-RF2_y)", "abs(F2R_y - RF2_y) < 2");
	c.cd(++pad_id);
	intersections->Draw("(F2R_z-RF2_z)", "abs(F2R_z - RF2_z) < 30");
}
