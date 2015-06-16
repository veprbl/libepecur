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
#include <TStyle.h>

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
#include "/home/veprbl/libepecur/contrib/rootlogon.C"

void	MomentumDistributionApp::PlotResults( uint chambers[][WIRES_COUNT] )
{
rootlogon();
gStyle->SetPaperSize(6,6);
	main_canvas = new TCanvas("main_canvas", ApplicationName(), 600, 600);

	int	rows_count = floor(sqrt(MAX_CHAMBER_ID));
	int	cols_count = ceil(MAX_CHAMBER_ID / (float)rows_count);

	for(chamber_id_t chamber_id = 0; chamber_id < MAX_CHAMBER_ID; chamber_id++)
	{
if (chamber_id != 6) continue;
		string	name = boost::lexical_cast<string>(chamber_id);
		string	title = ";X_{1^{\\text{й}}\\text{ фокус}}\\text{, мм}";
		TH1I*	hist = new TH1I(name.c_str(), title.c_str(), WIRES_COUNT, -WIRES_COUNT/2, WIRES_COUNT/2);


		for(int wire_id = 0; wire_id < WIRES_COUNT; wire_id++)
		{
			hist->SetBinContent(hist->FindBin(wire_id - WIRES_COUNT/2), chambers[chamber_id][wire_id]);
		}

		hist->Draw();
		main_canvas->SaveAs("out.tex");
		th.push_back(hist);
	}

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
	EvSumHook	hook(geom);

	loadfile(data_filepath, hook);

	PlotResults(hook.chambers);
}

int	main( int argc, char* argv[] )
{
	MomentumDistributionApp	app("Momentum distribution", argc, argv);

	while(1) { app.Run(); };

	return 0;
}
