#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TApplication.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TPaveText.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

#include "AxialShiftApp.hpp"
#include "AxialShiftHook.hpp"

namespace po = boost::program_options;

using namespace std;


AxialShiftApp::AxialShiftApp( const char* app_name, int &argc, char* argv[] )
	: TApplication(app_name, &argc, argv, nullptr, -1)
{
	ParseCommandLine();

	Init();
}

AxialShiftApp::~AxialShiftApp()
{
	delete hook;

	for(auto hist_tuple : shift_hist)
	{
		TH1I	*hist_ptr = hist_tuple.second;

		delete hist_ptr;
	}

	delete title_text_widget;
	delete hist_pad;
	delete main_canvas;
}

void	AxialShiftApp::ParseCommandLine()
{
	po::options_description cmdline_options;


	po::options_description visible("Allowed options");

	visible.add_options()
		("help,h", "produce this output")
		("geometry-file,g", po::value<string>(), "specify geometry description file")
		;
	cmdline_options.add(visible);

	po::positional_options_description p;
	p.add("input-file", 1);

	po::options_description hidden("Hidden options");

	hidden.add_options()
		("input-file", po::value< vector<string> >(), "input file")
		;
	cmdline_options.add(hidden);

	po::variables_map vm;

	po::store(po::command_line_parser(Argc(), Argv()).
		  options(cmdline_options).positional(p).run(), vm);

	po::notify(vm);

	if (vm.count("help") || !vm.count("input-file") || !vm.count("geometry-file"))
	{
		cerr << "Usage: " << Argv(0) << " data-file -g geometry-file [options]" << endl;
		cerr << endl;
		cerr << visible << endl;
		Terminate(1);
	}

	data_filepath = vm["input-file"].as< vector<string> >()[0];
	geometry_filepath = vm["geometry-file"].as<string>();
}

void	AxialShiftApp::Init()
{
	ifstream	file(geometry_filepath, ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);

	init_hists(geom);

	hook = new AxialShiftHook(geom, shift_hist);

	loadfile(data_filepath, *hook);

	cout << "Track miss count (chamber -> count -> part)" << endl;

	for(auto tup : hook->track_miss_count)
	{
		cout << int(tup.first) << " ->\t" << tup.second << "->\t" << tup.second/float(hook->track_count) << endl;
	}

	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	string	title_text = data_filepath + ": " + GIT_COMMIT_ID;

	title_text_widget = new TPaveText(0.3, 0.945, 0.7, 0.99);
	title_text_widget->AddText(title_text.c_str());
	title_text_widget->Draw();

	hist_pad = new TPad("hist_pad", "title", 0, 0, 1, 0.94);
	hist_pad->Draw();

	int	rows_count = floor(sqrt(shift_hist.size()));
	int	cols_count = ceil(shift_hist.size() / rows_count);

	hist_pad->Divide(rows_count, cols_count, 0, 0);

	int	pad_id = 1;

	for(auto chamb_tup : shift_hist)
	{
		TH1I*	hist = chamb_tup.second;

		hist_pad->cd(pad_id);
		hist->Draw();

		pad_id++;
	}
}

void	AxialShiftApp::init_hists( Geometry &geom )
{
	for(auto gr_tup : geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;

		for(auto axis_tup : gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			for(chamber_id_t chamber_id : axis_tup.second)
			{
				plane_id_t	plane_id = geom.chamber_plane[chamber_id];
				string	hist_name = "F" + boost::lexical_cast<string>(int(group_id)) + (axis == DEV_AXIS_X ? "X" : "Y") + boost::lexical_cast<string>(int(plane_id));

				shift_hist[chamber_id] = new TH1I(hist_name.c_str(), hist_name.c_str(), 20, -30.0, 30.0);
			}
		}

	}
}

int	main( int argc, char* argv[] )
{
	AxialShiftApp	app("Axial shift", argc, argv);
	while(1) { app.Run(); };

	return 0;
}
