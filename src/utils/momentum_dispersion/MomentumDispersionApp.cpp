#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <cmath>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1I.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>
#include <epecur/git-rev-export.hpp>

#include "MomentumDispersionApp.hpp"
#include "EvSumHook.hpp"

namespace po = boost::program_options;

using namespace std;

MomentumDistributionApp::MomentumDistributionApp( const char* app_name, int &argc, char* argv[] )
	: TApplication(app_name, &argc, argv, nullptr, -1)
{
	ParseCommandLine();

	Init();
}

MomentumDistributionApp::~MomentumDistributionApp()
{
	BOOST_FOREACH(auto hist, th)
	{
		delete(hist);
	}
	delete(main_canvas);
}

void	MomentumDistributionApp::ParseCommandLine()
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

void	MomentumDistributionApp::PlotResults(EvSumHook &hook)
{
	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	main_canvas->Divide(4);
	main_canvas->cd(1);
	hook.h1.Draw();
	main_canvas->cd(2);
	hook.h2.Draw();
	main_canvas->cd(3);
	hook.h3.Draw();
	main_canvas->cd(4);
	hook.h4.Draw();
	main_canvas->Show();
}

void	MomentumDistributionApp::Init()
{
	ifstream	file(geometry_filepath.c_str(), ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);
	EvSumHook	*hook = new EvSumHook(geom);

	loadfile("/data1/data_apr10/22041028.dat", *hook);
	loadfile("/data1/data_apr10/22041029.dat", *hook);
	loadfile("/data1/data_apr10/22041030.dat", *hook);
	loadfile("/data1/data_apr10/22041031.dat", *hook);

	PlotResults(*hook);
}

int	main( int argc, char* argv[] )
{
	MomentumDistributionApp	app("Momentum distribution", argc, argv);

	while(1) { app.Run(); };

	return 0;
}
