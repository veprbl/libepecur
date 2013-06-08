#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TApplication.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TPaveText.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

#include "../../../contrib/rootlogon.C"

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

	BOOST_FOREACH(auto hist_tuple, shift_hist)
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
	ifstream	file(geometry_filepath.c_str(), ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);

	init_hists(geom);

	hook = new AxialShiftHook(geom, shift_hist);

	try
	{
		loadfile(data_filepath, *hook);
	}
	catch(const char *e)
	{
		cerr << "Error: " << e << endl;
	}

	cout << "Track miss count (chamber -> count -> part)" << endl;

	BOOST_FOREACH(auto tup, hook->track_miss_count)
	{
		cout << int(tup.first) << " ->\t" << tup.second << "->\t" << tup.second/float(hook->track_count) << endl;
	}

	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	int	rows_count = 4;
	int	cols_count = ceil(shift_hist.size() / (float)rows_count);

	main_canvas->Divide(rows_count, cols_count, 0.005, 0.005);

	int	pad_id = 1;

	BOOST_FOREACH(auto chamb_tup, shift_hist)
	{
		TH1I*	hist = chamb_tup.second;
		double	shift_delta = -hist->GetMean();

		cerr << hist->GetTitle() << ".delta = " << shift_delta << endl;
	}

	BOOST_FOREACH(auto chamb_tup, shift_hist)
	{
		chamber_id_t	chamber_id = chamb_tup.first;
		TH1I*	hist = chamb_tup.second;
		double	new_shift_value = geom.chamber_axial_shift[chamber_id] - hist->GetMean();

		main_canvas->cd(pad_id);

		cout << "// " << hist->GetTitle() << ".axial_shift = " << new_shift_value << endl;

		hist->Draw();

		pad_id++;
	}
}

void	AxialShiftApp::init_hists( Geometry &geom )
{
	init_babar_style();
	gROOT->SetStyle("BABAR");
	gStyle->SetOptTitle(1);
	init_custom_palette();

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type != DEV_TYPE_PROP)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;

			BOOST_FOREACH(chamber_id_t chamber_id, axis_tup.second)
			{
				plane_id_t	plane_id = geom.chamber_plane[chamber_id];

				if ((group_id != 1) || (axis != DEV_AXIS_X))
				{
					continue;
				}

				string	hist_name = "F" + boost::lexical_cast<string>(int(group_id)) + (axis == DEV_AXIS_X ? "X" : "Y") + boost::lexical_cast<string>(int(plane_id));

				shift_hist[chamber_id] = new TH1I(hist_name.c_str(), hist_name.c_str(), 50, -5.0, 5.0);
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
