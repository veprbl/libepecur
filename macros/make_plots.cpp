#include <cstdlib>
#include <string>
#include <iostream>

#include <TClass.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>

#include "utils.C"

using std::string;

void	adjustment_shifts();
void	drift_calib();
void	drift_calib_curve();
void	drift_chisq();
void	drift_efficiency();
void	drift_efficiency_over_time();
void	drift_sum_rule();
void	target_info();
void	track_intersections();

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

bool	is_valid_file_pair()
{
	bool	result;
	const char *val1 = NULL;
	{
		TFile    f(gSystem->Getenv("EPECUR_ROOTFILE"), "READ");
		val1 = strdup(get_info((TTree*)f.Get("info"), "INPUT_FILE"));
	}
	{
		TFile    f(gSystem->Getenv("EPECUR_ROOTFILE2"), "READ");
		result =
		    strcmp(get_info((TTree*)f.Get("info"), "INPUT_FILE"), val1) == 0;
	}
	free((void*)val1);
	return result;
}

int	main(void)
{
	if (!is_valid_file_pair())
	{
		std::cerr << "Error: ROOT files were derrived from different data files" << std::endl;
		return EXIT_FAILURE;
	}

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
	out.mkdir("drift_efficiency")->cd();
	drift_efficiency();
	out.mkdir("drift_efficiency_over_time")->cd();
	drift_efficiency_over_time();
	out.mkdir("drift_sum_rule")->cd();
	drift_sum_rule();
	out.mkdir("target_info")->cd();
	target_info();
	out.mkdir("track_intersections")->cd();
	track_intersections();

	SaveAllAs(&out, c, "./");
	out.Write();

	return EXIT_SUCCESS;
}
