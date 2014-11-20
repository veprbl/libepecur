#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>

#include <epecur/geometry.hpp>
#include <epecur/git-rev-export.hpp>

#include "Process2ndPass.hpp"
#include "ProcessApp.hpp"
#include "ProcessMain.hpp"

namespace po = boost::program_options;

using namespace std;

string geometry_filepath;
string input_filepath;
string output_filepath;
double central_momentum;

void	ParseCommandLine( int argc, char* argv[] )
{
	po::options_description cmdline_options;


	po::options_description visible("Allowed options");

	visible.add_options()
		("help,h", "produce this output")
		("geometry-file,g", po::value<string>(), "specify geometry description file")
		("output-file,o", po::value<string>(), "output filepath")
		("central-momentum,m", po::value<double>(), "central momentum")
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

	if (vm.count("central-momentum"))
	{
		central_momentum = vm["central-momentum"].as<double>();
	}
	else
	{
		cerr << "Please specify central momentum option" << endl;
		cerr << visible << endl;
		exit(1);
	}

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

bool	is_same_size_tree(TTree *tree, Long_t *prev_entries, bool *first)
{
	Long_t entries;

	TObjArray *ar = tree->GetListOfBranches();
	for(Long_t i = 0; i < ar->GetEntries(); i++)
	{
		TBranch *br = (TBranch*)(*ar)[i];
		entries = br->GetEntries();

		if ((!*first) && (entries != *prev_entries))
		{
			return false;
		}
		*prev_entries = entries;
		*first = false;
	}
	return true;
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
	TFile		input_file(input_filepath.c_str());

	TTree	*info = (TTree*)input_file.FindObjectAny("info");
	TTree	*events = (TTree*)input_file.FindObjectAny("events");
	TTree	*cycle_efficiency = (TTree*)input_file.FindObjectAny("cycle_efficiency");

	// Now check that all branches in all tress have the same count of entries
	Long_t prev_entries;
	bool first = true;
	bool result = true;
	result &= is_same_size_tree(events, &prev_entries, &first);
	result &= is_same_size_tree(cycle_efficiency, &prev_entries, &first);
	if (!result)
	{
		throw "Unbalanced input tree";
	}

	if (!info)
	{
		throw "Missing info tree!";
	}

	info->Scan("key:value", "", "colsize=30");

	map<string, string>	info_hash = get_info_hash(info);
	string file_commit_id = info_hash["GIT_COMMIT_ID"];
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
	string original_filename =
	    gSystem->BaseName(info_hash["INPUT_FILE"].c_str());
	cerr << "original_filename\t" << original_filename << endl;

	try
	{
		TFile	output_file(output_filepath.c_str(), "RECREATE");
		TTree	*events_new;
		TTree	*efficiency_tree;
		TTree	*info_new = info->CloneTree();
		add_info_value(info_new, "PROCESS_GIT_COMMIT_ID", GIT_COMMIT_ID);
		intersection_set_t	s;

		events_new = Process(events, cycle_efficiency, geom, central_momentum, &s);

		efficiency_tree = Process2ndPass(events_new);

		// Now check that all branches in all tress have the same count of entries
		Long_t prev_entries;
		bool first = true;
		bool result = true;
		result &= is_same_size_tree(events_new, &prev_entries, &first);
		result &= is_same_size_tree(efficiency_tree, &prev_entries, &first);
		if (!result)
		{
			throw "Unbalanced output tree";
		}

		output_file.Write();
	}
	catch(const char *e)
	{
		cerr << "Exception: " << e << endl;
		cerr << "Removing output file \"" << output_filepath << "\"." << endl;
		gSystem->Unlink(output_filepath.c_str());
		return EXIT_FAILURE;
	}

	return 0;
}
