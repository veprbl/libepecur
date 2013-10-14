#include <cstring>

#include <TFile.h>
#include <TCanvas.h>
#include <TSpectrum.h>

using std::string;

TCanvas	c;
TFile	f("26061082.root", "READ");
int		pad_id = 0;

void	plot_intersection_per_coord(string i, int xmin, int xmax, int rms_max)
{
	TH1F	*uu1 = new TH1F(("uu1_" + i).c_str(), ("LX_" + i).c_str(),
	                        xmax - xmin, xmin, xmax);
	TH1F	*uu2 = new TH1F(("uu2_" + i).c_str(), ("RX_" + i).c_str(),
	                        xmax - xmin, xmin, xmax);
	TSpectrum	*sl = new TSpectrum(3);
	TSpectrum	*sr = new TSpectrum(3);

	c.cd(++pad_id);
	intersections->Draw(("LP_" + i + " >> uu1_" + i).c_str());
	if (i == "x")
	{
		uu1->Smooth(10);
		assert(sl->Search(uu1, 1, "") == 3);
	}
	c.cd(++pad_id);
	intersections->Draw(("RP_" + i + " >> uu2_" + i).c_str());
	if (i == "x")
	{
		uu2->Smooth(10);
		assert(sr->Search(uu2, 1, "") == 3);
	}
	c.cd(++pad_id);
	TH1F	*uu3 = new TH1F(*uu1);
	uu3->Add(uu2, -1);
	uu3->Draw();
	c.cd(++pad_id);
	char	func_str[256], cut_str[256];
	snprintf(func_str, sizeof(func_str),
		"LP_%s - RP_%s",
		i.c_str(), i.c_str()
		);
	snprintf(
		cut_str, sizeof(cut_str),
		"(LP_%s > %i) && (LP_%s < %i) && (RP_%s > %i) && (RP_%s < %i)"
		" && (abs(%s) < %i)",
		i.c_str(), xmin,
		i.c_str(), xmax,
		i.c_str(), xmin,
		i.c_str(), xmax,
		func_str, rms_max
		);
	intersections->Draw(func_str, cut_str);
	if (i == "x")
	{
		double l1 = sl->GetPositionX()[2];
		double l2 = sl->GetPositionX()[0];
		double r1 = sr->GetPositionX()[2];
		double r2 = sr->GetPositionX()[0];
		assert(l1 < l2);
		assert(r1 < r2);
		printf("\nPeak positions:\n\tLeft plot:\t%f\t%f\n\tRight plot:\t%f\t%f\n\n",
		       l1, l2, r1, r2);
		printf("Peak alignment plan:\n\tMove by:\t%f\n\tScale by:\t%f\n\n",
		       (l1 + l2 - r1 - r2) / 2,
		       (l2 - l1) / (r2 - r1));
	}
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

	plot_intersection_per_coord("x", -500, 300, 150);
	plot_intersection_per_coord("z", -30, 30, 30);
}
