#include <TFile.h>
#include <TCanvas.h>

TCanvas	c;
TFile	f("26061082.root", "READ");
int		pad_id = 0;

/*!
 * This script is showing coordinates for drift track section with
 * some plane. These distribution should match when adjustment is done
 * correctly.
 * Plot in the right column is showing coordinate difference for these
 * intersection points, which look almost the same as in adjustment_shifts.C
 */
void	track_plane_section()
{
	c.Divide(3, 2);
	c.Show();

	c.cd(++pad_id);
	intersections->Draw("LP_x", "(LP_x > -500) && (LP_x < 300)");
	c.cd(++pad_id);
	intersections->Draw("RP_x", "(RP_x > -500) && (RP_x < 300)");
	c.cd(++pad_id);
	intersections->Draw("LP_x - RP_x", "(LP_x > -500) && (LP_x < 300) && (RP_x > -500) && (RP_x < 300)");

	c.cd(++pad_id);
	intersections->Draw("LP_z", "abs(LP_z) < 30");
	c.cd(++pad_id);
	intersections->Draw("RP_z", "abs(RP_z) < 30");
	c.cd(++pad_id);
	intersections->Draw("LP_z - RP_z", "(abs(LP_z) < 30) && (abs(RP_z) < 30)");
}
