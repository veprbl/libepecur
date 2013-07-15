#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>
#include <epecur/git-rev-export.hpp>

#include "TreeExportApp.hpp"
#include "TreeExportHook.hpp"
#include "DriftCalibHook.hpp"

namespace po = boost::program_options;

using namespace std;

string data_filepath;
string geometry_filepath;
string output_filepath;
bool rough_drift;

void	ParseCommandLine( int argc, char* argv[] )
{
	po::options_description cmdline_options;


	po::options_description visible("Allowed options");

	visible.add_options()
		("help,h", "produce this output")
		("geometry-file,g", po::value<string>(), "specify geometry description file")
		("output-file,o", po::value<string>(), "specify output file")
		("rough-drift", "disable using time information for drift tracks")
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

	if (vm.count("help") || !vm.count("input-file") || !vm.count("geometry-file") || !vm.count("output-file"))
	{
		cerr << "Usage: " << argv[0] << " data-file -g geometry-file -o output-file [options]" << endl;
		cerr << endl;
		cerr << visible << endl;
		exit(1);
	}

	data_filepath = vm["input-file"].as< vector<string> >()[0];
	geometry_filepath = vm["geometry-file"].as<string>();
	output_filepath = vm["output-file"].as<string>();
	rough_drift = vm.count("rough-drift");
}

void	fill_info( TTree &info )
{
	char	key[MAX_VALUE_LEN];
	char	value[MAX_VALUE_LEN];

	info.Branch("key", key, "key/C");
	info.Branch("value", value, "value/C");

	strncpy(key, "GIT_COMMIT_ID", MAX_VALUE_LEN);
	strncpy(value, GIT_COMMIT_ID, MAX_VALUE_LEN);
	info.Fill();

	strncpy(key, "INPUT_FILE", MAX_VALUE_LEN);
	strncpy(value, data_filepath.c_str(), MAX_VALUE_LEN);
	info.Fill();

	strncpy(key, "GEOMETRY_FILE", MAX_VALUE_LEN);
	strncpy(value, geometry_filepath.c_str(), MAX_VALUE_LEN);
	info.Fill();
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
	TFile		tree_file(output_filepath.c_str(), "RECREATE");
	TTree		info("info", "information about this file");
	StdDrift::calibration_curve_t	*calibration_curve = NULL;

	DriftCalibHook	calib_hook(geom);

	if (!rough_drift)
	{
		try
		{
			loadfile(data_filepath, calib_hook);
		}
		catch( const char *err )
		{
			cerr << err << endl;
		}

		calib_hook.generate_calibration_curves();
		calibration_curve = &calib_hook.calibration_curve;
	}

	TreeExportHook	hook(geom, calibration_curve);

	try
	{
		loadfile(data_filepath, hook);
	}
	catch( const char *err )
	{
		cerr << err << endl;
	}

	fill_info(info);

	tree_file.Write();

	return 0;
}
