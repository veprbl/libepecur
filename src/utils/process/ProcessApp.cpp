#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <TApplication.h>
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
string output_filepath;

void	ParseCommandLine( int argc, char* argv[] )
{
	po::options_description cmdline_options;


	po::options_description visible("Allowed options");

	visible.add_options()
		("help,h", "produce this output")
		("geometry-file,g", po::value<string>(), "specify geometry description file")
		("output-file,o", po::value<string>(), "output filepath")
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
		cerr << "Usage: " << argv[0] << " input-file -g geometry-file -o output-file" << endl;
		cerr << endl;
		cerr << visible << endl;
		exit(1);
	}

	input_filepath = vm["input-file"].as< vector<string> >()[0];
	output_filepath = vm["output-file"].as<string>();
	geometry_filepath = vm["geometry-file"].as<string>();

	if (output_filepath == input_filepath)
	{
		cerr << "Output filename is identical to input filename." << endl;
		exit(1);
	}
}

map<string, string>	get_info_hash( TTree *info )
{
	map<string, string>	result;
	TBranch	*key_br = info->GetBranch("key");
	TBranch	*value_br = info->GetBranch("value");
	void	*key_adr = key_br->GetAddress();
	void	*value_adr = value_br->GetAddress();
	char    key[MAX_VALUE_LEN];
	char    value[MAX_VALUE_LEN];

	key_br->SetAddress(key);
	value_br->SetAddress(value);

	int count = info->GetEntries();
	for(int i = 0; i < count; i++)
	{
		info->GetEntry(i);
		result[key] = value;
	}

	key_br->SetAddress(key_adr);
	value_br->SetAddress(value_adr);

	return result;
}

void	add_info_value( TTree *info, string key, string value )
{
	info->GetBranch("key")->SetAddress((void*)key.c_str());
	info->GetBranch("value")->SetAddress((void*)value.c_str());
	info->Fill();
}

int	main( int argc, char* argv[] )
{
	ParseCommandLine(argc, argv);

	TApplication	app("Event visualizer", &argc, argv);
	ifstream	file(geometry_filepath.c_str(), ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);
	TFile		input_file(input_filepath.c_str());

	TTree	*info = (TTree*)input_file.FindObjectAny("info");
	TTree	*events = (TTree*)input_file.FindObjectAny("events");

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

	TFile	output_file(output_filepath.c_str(), "RECREATE");
	boost::scoped_ptr<TTree>	events_new;
	boost::scoped_ptr<TTree>	info_new(info->CloneTree());
	add_info_value(info_new.get(), "PROCESS_GIT_COMMIT_ID", GIT_COMMIT_ID);
	intersection_set_t	s;

	Process(events, &vis_result, &s, events_new);

	events_new->AutoSave();
	info_new->AutoSave();
	input_file.Close();
	output_file.Close();

	ProcessVisualize();

	while(1) { app.Run(); };

	return 0;
}
