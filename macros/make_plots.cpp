#include <cstdlib>
#include <string>

#include <TClass.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>

using std::string;

void	adjustment_shifts();
void	drift_calib();
void	drift_calib_curve();
void	drift_chisq();
void	drift_effectivity();
void	track_intersections();
void	track_plane_section();

bool	checkoption(char *hoptions, const char *option)
{
	char *pos = strcasestr(hoptions, option);
	if (pos)
	{
		memset(pos, ' ', strlen(option));
	}
	return bool(pos);
}

void SaveAllAs(TDirectory *dir, TCanvas &default_canvas, const string &path)
{
	if (gSystem->AccessPathName(path.c_str(), kFileExists))
	{
		if (gSystem->MakeDirectory(path.c_str()) != 0)
		{
			gSystem->Error("MakeDirectory", "Can't create %s", path.c_str());
			return;
		}
	}

	TIter it(dir->GetList());
	TObject	*obj;
	while(( obj = it() ))
	{
		if (obj->IsA()->InheritsFrom("TH1"))
		{
			TH1		*hist = (TH1*)obj;
			char	*hoptions = (char*)hist->GetOption();

			if (strcasestr(hoptions, "NOIMG")) continue;

			default_canvas.SetLogz(checkoption(hoptions, "LOGZ"));
			default_canvas.SetLogx(checkoption(hoptions, "XX"));
			default_canvas.SetLogy(checkoption(hoptions, "XY"));
			default_canvas.SetGridx(checkoption(hoptions, "GRIDX"));
			default_canvas.SetGridy(checkoption(hoptions, "GRIDY"));

			if (checkoption(hoptions, "DUMMY"))
			{
				hist->SetStats(kFALSE);
			}

			default_canvas.cd();
			hist->Draw(hoptions);
			default_canvas.Update();

			string file_name = path + "/" + string(hist->GetName()) + ".png";
			default_canvas.SaveAs(file_name.c_str());
		}
		else if (obj->IsA()->InheritsFrom("TDirectory"))
		{
			TDirectory	*subdir = (TDirectory*)obj;
			string	newpath = path + "/" + obj->GetName();

			SaveAllAs(subdir, default_canvas, newpath);
		}
	}
}

int	main(void)
{
	TCanvas	c;
	TFile	out("out.root", "RECREATE");

	out.mkdir("adjustment_shifts")->cd();
	adjustment_shifts();
	out.mkdir("drift_calib")->cd();
	drift_calib();
	out.mkdir("drift_calib_curve")->cd();
	drift_calib_curve();
	out.mkdir("drift_chisq")->cd();
	drift_chisq();
	out.mkdir("drift_effectivity")->cd();
	drift_effectivity();
	out.mkdir("track_intersections")->cd();
	track_intersections();
	out.mkdir("track_plane_section")->cd();
	track_plane_section();

	SaveAllAs(&out, c, "./");
	out.Write();

	return EXIT_SUCCESS;
}
