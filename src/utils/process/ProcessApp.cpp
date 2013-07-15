#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>

#include <epecur/geometry.hpp>
#include <epecur/git-rev-export.hpp>

#include "ProcessApp.hpp"
#include "ProcessMain.hpp"
#include "ProcessVisualizer.hpp"

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

map<string, string>	get_info_hash( TTree *info )
{
	map<string, string>	result;
	char    key[MAX_VALUE_LEN];
	char    value[MAX_VALUE_LEN];

	info->GetBranch("key")->SetAddress(key);
	info->GetBranch("value")->SetAddress(value);

	int count = info->GetEntries();
	for(int i = 0; i < count; i++)
	{
		info->GetEntry(i);
		result[key] = value;
	}

	return result;
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

	TTree	*info = (TTree*)tree_file.FindObjectAny("info");

	if (!info)
	{
		throw "Missing info tree!";
	}

	info->Scan("key:value", "", "colsize=30");

	string file_commit_id = get_info_hash(info)["GIT_COMMIT_ID"];
	if (file_commit_id != GIT_COMMIT_ID)
	{
		cerr << endl
		     << "Warning: Commit id mismatch"
		     << endl
		     << "File created by: "
		     << file_commit_id
		     << endl
		     << "Current software is: "
		     << GIT_COMMIT_ID
		     << endl;
	}

	if (tree_file.FindObjectAny("intersections"))
	{
		cerr << "Deleting previous data" << endl;
		tree_file.ReOpen("UPDATE");
		tree_file.Delete("intersections;*");
		tree_file.ReOpen("READ");

		if (tree_file.FindObjectAny("intersections"))
		{
			cerr << "Delete didn't work" << endl;
			throw;
		}
	}

	TTree	intersections("intersections", "Track intersections");
	intersection_set_t	s;

	s.br_lr = intersections.Branch("LR", &s.i_lr, "LR_x/D:LR_y/D:LR_z/D");
	s.br_rl = intersections.Branch("RL", &s.i_rl, "RL_x/D:RL_y/D:RL_z/D");
//  Not enabled yet
//	s.br_f2r = intersections.Branch("F2R", &s.i_f2r, "F2R_x/D:F2R_y/D:F2R_z/D");
//	s.br_f2l = intersections.Branch("F2L", &s.i_f2l, "F2L_x/D:F2L_y/D:F2L_z/D");
//	s.br_rf2 = intersections.Branch("RF2", &s.i_rf2, "RF2_x/D:RF2_y/D:RF2_z/D");
//	s.br_lf2 = intersections.Branch("LF2", &s.i_lf2, "LF2_x/D:LF2_y/D:LF2_z/D");

	Process((TTree*)tree_file.FindObjectAny("events"), &vis_result, &s);

	tree_file.ReOpen("UPDATE");
	intersections.Write("intersections");
	tree_file.Close();

	return 0;
}
