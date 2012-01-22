#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1I.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>

#include "main.hpp"
#include "EvSumHook.hpp"

namespace po = boost::program_options;

using namespace std;

MomentumDistributionApp::MomentumDistributionApp(
	const char* appClassName, Int_t* argc, char** argv, void* options, Int_t numOptions
	)
		: TApplication(appClassName, nullptr, nullptr, options, numOptions)
{
	ParseCommandLine(*argc, argv);

	Init();
}

MomentumDistributionApp::~MomentumDistributionApp()
{
	for(auto hist: th)
	{
		delete(hist);
	}
	delete(main_canvas);
}

void	MomentumDistributionApp::ParseCommandLine( int argc, char* argv[] )
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

	po::store(po::command_line_parser(argc, argv).
		  options(cmdline_options).positional(p).run(), vm);

	po::notify(vm);

	if (vm.count("help") || !vm.count("input-file") || !vm.count("geometry-file"))
	{
		cerr << "Usage: " << argv[0] << " data-file -g geometry-file [options]" << endl;
		cerr << endl;
		cerr << visible << endl;
		Terminate(1);
	}

	data_filepath = vm["input-file"].as< vector<string> >()[0];
	geometry_filepath = vm["geometry-file"].as<string>();
}

void	MomentumDistributionApp::PlotResults( uint planes[][WIRES_COUNT] )
{
	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	int	rows_count = floor(sqrt(MAX_PLANE_ID));
	int	cols_count = ceil(MAX_PLANE_ID / rows_count);

	main_canvas->Divide(rows_count, cols_count);

	for(int plane_id = 0; plane_id < MAX_PLANE_ID; plane_id++)
	{
		string	name = boost::lexical_cast<string>(plane_id);
		string	title = geometry_filepath + ":" + name + " " + GIT_COMMIT_ID;
		TH1I*	hist = new TH1I(name.c_str(), title.c_str(), WIRES_COUNT, -WIRES_COUNT/2, WIRES_COUNT/2);

		main_canvas->cd(plane_id + 1);

		for(int wire_id = 0; wire_id < WIRES_COUNT; wire_id++)
		{
			hist->Fill(wire_id - WIRES_COUNT/2, planes[plane_id][wire_id]);
		}

		hist->Draw();
		th.push_back(hist);
	}

	main_canvas->Show();
}

void	MomentumDistributionApp::Init()
{
	ifstream	file(geometry_filepath, ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);
	EvSumHook	hook(geom);

	loadfile(data_filepath, hook);

	PlotResults(hook.planes);
}

int	main( int argc, char* argv[] )
{
	MomentumDistributionApp	app("Momentum distribution", &argc, argv);

	while(1) { app.Run(); };

	return 0;
}
