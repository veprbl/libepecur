#include <string>

#include <TFile.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TTree.h>
#include <TH2F.h>

using std::string;

static TCanvas	c;
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE2"), "READ");
static int		pad_id = 0;
static TTree	*events;

static void	plot_one_intersection_per_coord(string i, string j, string id)
{
	int	nbins, ymin, ymax;

	if (i == "x")
	{
		nbins = 300;
		ymin = -500;
		ymax = 300;
	}
	else
	{
		nbins = 80;
		ymin = -40;
		ymax = 40;
	}

	string	hist_name = id + i + j;
	c.cd(++pad_id);
	TH2F	*h =  new TH2F(
		hist_name.c_str(),
		(hist_name + ";" + (i + ", [mm];") + j + ", [mm]").c_str(),
		80, -40, 40, nbins, ymin, ymax
		);
	h->SetOption("zcol");
	events->Draw(
		(id + "_" + i + ":" + id + "_" + j + " >> " + hist_name).c_str()
		);
}

static void	plot_intersections_per_coords(string i, string j)
{
	plot_one_intersection_per_coord(i, j, "LF2");
	plot_one_intersection_per_coord(i, j, "RF2");
	plot_one_intersection_per_coord(i, j, "LR");
	plot_one_intersection_per_coord(i, j, "F2L");
	plot_one_intersection_per_coord(i, j, "F2R");
	plot_one_intersection_per_coord(i, j, "RL");
}

/*!
 * This script is showing coordinate distributions for coordinates of
 * closest points lying on different tracks.
 */
void	track_intersections()
{
	events = (TTree*)f.Get("events");

	c.Divide(3, 6);
	c.Show();

	plot_intersections_per_coords("x", "y");
	plot_intersections_per_coords("x", "z");
	plot_intersections_per_coords("y", "z");
}
