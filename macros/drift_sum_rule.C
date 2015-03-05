#include <cmath>

#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TLine.h>
#include <TMath.h>
#include <TSystem.h>
#include <TTree.h>

static const int N = 5;
const int NUM_CHAMBERS = 4;
static TCanvas	c("c", "", 1600.0, 1600.0 * NUM_CHAMBERS / 2 / N);
static TFile	f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
static TTree	*events;

double	calc_AB(double alpha)
{
	return sin((90 - alpha) * TMath::DegToRad()) * 17.0;
}

double	calc_DE(double alpha)
{
	if (alpha < -30) return NAN;
	return sin((alpha + 30) * TMath::DegToRad()) * 17.0;
}

double	calc_AC(double alpha)
{
	return sin((-alpha + 30) * TMath::DegToRad()) * 17.0;
}

void	mk_plot(int i, const char *chamber, int plane_id, bool select_same_plane, bool time_domain, double angle, double angle_bin_width)
{
	if ((plane_id != 1) && (plane_id != 3)) throw;

	char *hname = Form("h%s_%i%i_%i", chamber, plane_id, plane_id + 1, i);
	double bin_count, bin_min, bin_max;
	const char *var1, *var2, *units, *adjacent_selector;
	if (time_domain)
	{
		bin_count = 380;
		bin_min = 0.0;
		bin_max = 380;
	}
	else
	{
		bin_count = 100;
		bin_min = 0.0;
		bin_max = 8.5;
	}
	TH2F *h = new TH2F(hname, Form("angle = %.0f \\pm %.0f", angle, angle_bin_width / 2), bin_count, bin_min, bin_max, bin_count, bin_min, bin_max);
	h->SetStats(kFALSE);
	if (time_domain)
	{
		units = "counts";
		var1 = "d%s%i_time[0]";
		if (select_same_plane)
		{
			var2 = Form("d%%s%i_time[1]", plane_id);
		}
		else
		{
			var2 = Form("d%%s%i_time[0]", plane_id + 1);
		}
	}
	else
	{
		units = "mm";
		var1 = "d%s%i_drift_offset[0]";
		if (select_same_plane)
		{
			var2 = Form("d%%s%i_drift_offset[1]", plane_id);
		}
		else
		{
			var2 = Form("d%%s%i_drift_offset[0]", plane_id + 1);
		}
	}
	var1 = Form(var1, chamber, plane_id);
	var2 = Form(var2, chamber);
	if (select_same_plane)
	{
		adjacent_selector = "(d%s%i_num_wires == 2)"
		" && (abs(abs(d%s%i_wire_pos[0] - d%s%i_wire_pos[1]) - 2.0) < 0.0000000001)";
		adjacent_selector = Form(adjacent_selector, chamber, plane_id, chamber, plane_id, chamber, plane_id);
	}
	else
	{
		adjacent_selector = "(d%s%i_num_wires == 1) && (d%s%i_num_wires == 1)"
		" && (abs(abs(d%s%i_wire_pos[0] - d%s%i_wire_pos[0]) - 1.0) < 0.0000000001)";
		adjacent_selector = Form(adjacent_selector, chamber, plane_id, chamber, plane_id + 1, chamber, plane_id, chamber, plane_id + 1);
	}
	h->SetXTitle(Form("%s, %s", var2, units));
	h->SetYTitle(Form("%s, %s", var1, units));
	events->Draw(
		Form("%s:%s >> %s", var1, var2, hname),
		Form(
			"%s"
			" && (t%s_track_count == 1)"
			" && (abs(abs(atan(t%s_c1[0])) - %f) < %f)",
			adjacent_selector, chamber, chamber, angle * TMath::DegToRad(), angle_bin_width / 2 * TMath::DegToRad()),
		"COL Z"
		);

	if (!time_domain)
	{
		if (select_same_plane)
		{
			// To be implemented
		}
		else
		{
			double AC, DE, dx;
			TLine *line_DE, *line_AC;

			DE = calc_DE(angle);
			dx = 8.5 - DE/2;
			line_DE = new TLine(DE/2+dx, DE/2-dx, DE/2-dx, DE/2+dx);
			line_DE->Draw();

			DE = calc_DE(-angle);
			dx = DE/2;
			line_DE = new TLine(DE/2+dx, DE/2-dx, DE/2-dx, DE/2+dx);
			line_DE->Draw();

			AC = calc_AC(angle);
			dx = 8.5 - AC;
			line_AC = new TLine(0, AC, dx, AC+dx);
			line_AC->Draw();
			line_AC = new TLine(AC, 0, AC+dx, dx);
			line_AC->Draw();
		}
	}
}

void	drift_sum_rule()
{
	events = (TTree*)f.Get("events");

	const char* chamber[NUM_CHAMBERS] = {"3X", "3Y", "4X", "4Y"};
	const double STEP = 30.0/(N-1);

	c.Divide(2 * N, NUM_CHAMBERS);
	int cell = 1;

	for(int chamber_id = 0; chamber_id < NUM_CHAMBERS; chamber_id++)
	{
		for(int plane_id = 1; plane_id <= 3; plane_id += 2)
		{
			for(int i = 1; i <= N; i++)
			{
				c.cd(cell++);
				double angle = STEP * (i - 1);
				mk_plot(i, chamber[chamber_id], plane_id, false, false, angle, 0.5 * STEP);
			}
		}
	}
}
