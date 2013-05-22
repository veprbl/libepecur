#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>

#include <epecur/geometry.hpp>

#include "ProcessApp.hpp"

namespace po = boost::program_options;

using namespace std;

string geometry_filepath;
string input_filepath;

void	ParseCommandLine( int argc, char* argv[] )
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
		cerr << "Usage: " << argv[0] << " input-file -g geometry-file" << endl;
		cerr << endl;
		cerr << visible << endl;
		exit(1);
	}

	input_filepath = vm["input-file"].as< vector<string> >()[0];
	geometry_filepath = vm["geometry-file"].as<string>();
}

int	main( int argc, char* argv[] )
{
	ParseCommandLine(argc, argv);

	ifstream	file(geometry_filepath.c_str(), ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);
	TFile		tree_file(input_filepath.c_str());

	return 0;
}
