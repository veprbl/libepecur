#include <cstring>

#include <TFile.h>
#include <TCanvas.h>

using std::string;

TCanvas	c;
TFile	f("26061082.root", "READ");
int		pad_id = 0;

void	plot_intersection_per_coord(string i, int xmin, int xmax)
{
	TH1F	*uu1 = new TH1F(("uu1_" + i).c_str(), ("LX_" + i).c_str(),
	                        xmax - xmin, xmin, xmax);
	TH1F	*uu2 = new TH1F(("uu2_" + i).c_str(), ("LX_" + i).c_str(),
	                        xmax - xmin, xmin, xmax);
	c.cd(++pad_id);
	intersections->Draw(("LP_" + i + " >> uu1_" + i).c_str());
	c.cd(++pad_id);
	intersections->Draw(("RP_" + i + " >> uu2_" + i).c_str());
	c.cd(++pad_id);
	TH1F	*uu3 = new TH1F(*uu1);
	uu3->Add(uu2, -1);
	uu3->Draw();
	c.cd(++pad_id);
	char	cut_str[256];
	snprintf(
		cut_str, sizeof(cut_str),
		"(LP_%s > %i) && (LP_%s < %i) && (RP_%s > %i) && (RP_%s < %i)",
		i.c_str(), xmin,
		i.c_str(), xmax,
		i.c_str(), xmin,
		i.c_str(), xmax
		);
	intersections->Draw(
		("LP_" + i + " - RP_" + i).c_str(),
		cut_str
		);
}

/*!
 * This script is showing coordinates for drift track section with
 * some plane. These distribution should match when adjustment is done
 * correctly.
 * Plot in the right column is showing coordinate difference for these
 * intersection points, which look almost the same as in adjustment_shifts.C
 */
void	track_plane_section()
{
	c.Divide(4, 2);
	c.Show();

	plot_intersection_per_coord("x", -500, 300);
	plot_intersection_per_coord("z", -30, 30);
}
