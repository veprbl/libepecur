#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>
#include <epecur/types.hpp>

#include "TreeExportApp.hpp"
#include "TreeExportHook.hpp"

namespace po = boost::program_options;

using namespace std;

string data_filepath;
string geometry_filepath;
string output_filepath;

void	ParseCommandLine( int argc, char* argv[] )
{
	po::options_description cmdline_options;


	po::options_description visible("Allowed options");

	visible.add_options()
		("help,h", "produce this output")
		("geometry-file,g", po::value<string>(), "specify geometry description file")
		("output-file,o", po::value<string>(), "specify output file")
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

unsigned int	plot_calib_curve(
	TreeExportHook &hook,
	chamber_id_t chamber_id,
	TH2F &calib_curve,
	TH1F &calib_chisq
	)
{
	unsigned int	num_events = 0;

	BOOST_FOREACH(auto &pair, hook.time_distributions[chamber_id])
	{
		small_angle_t	angle = pair.first;
		auto		&distribution = pair.second;
		uint16_t	time = 0;
		unsigned int	overal_integral = 0, integral = 0;

		BOOST_FOREACH(auto counts, distribution)
		{
			overal_integral += counts;
		}

		num_events += overal_integral;

		BOOST_FOREACH(auto counts, distribution)
		{
			integral += counts;
			calib_curve.Fill(angle, time,
					 integral / (float)overal_integral);

			time++;
		}
	}

	for(int time = 0; time < MAX_TIME_COUNTS; time++)
	{
		double	sum = 0;
		int	count = 0;

		BOOST_FOREACH(auto &pair, hook.time_distributions[chamber_id])
		{
			small_angle_t	angle = pair.first;
			auto	bin_id = calib_curve.FindBin(angle, time);
			auto	N = calib_curve.GetBinContent(bin_id);

			sum += N;
			++count;
		}

		double	mean = sum / (double)count;
		double	sum_sqr = 0;

		BOOST_FOREACH(auto &pair, hook.time_distributions[chamber_id])
		{
			wire_pos_t	wire_pos = pair.first;
			auto	bin_id = calib_curve.FindBin(wire_pos, time);
			auto	N = calib_curve.GetBinContent(bin_id);

			sum_sqr += (mean - N) * (mean - N);
		}

		double	chisq = sqrt(sum_sqr / (double)count);

		calib_chisq.Fill(time, chisq);
	}

	return num_events;
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
	TreeExportHook	hook(geom);

	try
	{
		loadfile(data_filepath, hook);
	}
	catch( const char *err )
	{
		cerr << err << endl;
	}

	fill_info(info);

	string	title;
	TTree	drift_calib("drift_calib", "drift chambers calibration curves");
	TH2F	calib_curve(
		"calib_curve", "",
		36, -90, 90,
		MAX_TIME_COUNTS + 5, 0, MAX_TIME_COUNTS + 5
		);
	TH1F	calib_chisq(
		"calib_chisq", "",
		MAX_TIME_COUNTS, 0, MAX_TIME_COUNTS
		);

	drift_calib.Branch("calib_curve", "TH2F", &calib_curve);
	drift_calib.Branch("calib_chisq", "TH1F", &calib_chisq);

	BOOST_FOREACH(auto gr_tup, geom.group_chambers)
	{
		group_id_t	group_id = gr_tup.first;
		device_type_t	device_type = geom.group_device_type[group_id];

		if (device_type != DEV_TYPE_DRIFT)
		{
			continue;
		}

		BOOST_FOREACH(auto axis_tup, gr_tup.second)
		{
			device_axis_t	axis = axis_tup.first;
			vector<chamber_id_t>	&chambers =
				geom.group_chambers[group_id][axis];
			int	chamber_num = 1;

			BOOST_FOREACH(chamber_id_t chamber_id, chambers)
			{
				auto	num_events = plot_calib_curve(
					hook,
					chamber_id,
					calib_curve,
					calib_chisq
					);

				title = "d" +
					boost::lexical_cast<string>(int(group_id)) +
					((axis == DEV_AXIS_X) ? "X" : "Y") +
					boost::lexical_cast<string>(int(chamber_num)) +
					" num_events = " +
					boost::lexical_cast<string>(num_events);

				calib_curve.SetTitle(title.c_str());
				calib_chisq.SetTitle(title.c_str());

				drift_calib.Fill();
				calib_curve.Reset();
				calib_chisq.Reset();

				chamber_num++;
			}
		}
	}

	tree_file.Write();

	return 0;
}
