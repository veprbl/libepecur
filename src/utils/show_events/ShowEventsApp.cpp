#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <TROOT.h>
#include <TApplication.h>
#include <TThread.h>
#include <TCanvas.h>
#include <TH1I.h>
#include <TLine.h>
#include <TButton.h>
#include <TPaveText.h>

#include <epecur/geometry.hpp>
#include <epecur/loadfile.hpp>

#include "ShowEventsApp.hpp"
#include "EvShowHook.hpp"

namespace po = boost::program_options;

using namespace std;

const auto	NEXT_EVENT_CMD = "next_event";
const auto	PREV_EVENT_CMD = "prev_event";


ShowEventsApp::ShowEventsApp( const char* app_name, int &argc, char* argv[] )
	: TApplication(app_name, &argc, argv, nullptr, -1)
{
	ParseCommandLine();

	Init();
}

ShowEventsApp::~ShowEventsApp()
{
	delete hook;
	for(auto line : wires_lines)
	{
		delete line;
	}
	delete title_text_widget;
	delete event_pad;
	delete main_canvas;
}

void	ShowEventsApp::ParseCommandLine()
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

void	ShowEventsApp::SetupButtonColors()
{
	if (displayed_event_id == LOAD_EVENTS_COUNT - 1)
	{
		next_button->SetFillColor(18);
	}
	else
	{
		next_button->SetFillColor(42);
	}

	if (displayed_event_id == 0)
	{
		prev_button->SetFillColor(18);
	}
	else
	{
		prev_button->SetFillColor(42);
	}
	next_button->Paint();
	prev_button->Paint();
}

void	ShowEventsApp::SetupTitle()
{
	string	event_id_str = boost::lexical_cast<string>(displayed_event_id + 1);

	title_text = data_filepath + ": event(" + event_id_str + ") " + GIT_COMMIT_ID;

	if (!title_text_widget)
	{
		delete title_text_widget;
	}

	main_canvas->cd();

	title_text_widget = new TPaveText(0.3, 0.9, 0.7, 1);
	title_text_widget->AddText(title_text.c_str());
	title_text_widget->Draw();
}

Long_t	ShowEventsApp::ProcessLine( const char* line, Bool_t sync, Int_t* error )
{
	if (strcmp(line, NEXT_EVENT_CMD) == 0)
	{
		if (displayed_event_id == LOAD_EVENTS_COUNT - 1)
		{
			return 0;
		}

		displayed_event_id++;
	}
	else if (strcmp(line, PREV_EVENT_CMD) == 0)
	{
		if (displayed_event_id == 0)
		{
			return 0;
		}

		displayed_event_id--;
	}
	else
	{
		cerr << "unexpected call ShowEventsApp::ProcessLine(\"" << line << "\")" << endl;
		return 0;
	}

	for(auto line : wires_lines)
	{
		delete line;
	}
	wires_lines.clear();

	SetupButtonColors();
	SetupTitle();

	PlotResults();

	return 0;
}

void	ShowEventsApp::PlotResults()
{
	event_pad->cd();

	for(uint plane_id = 0; plane_id < MAX_PLANE_ID; plane_id++)
	{
		for(uint i = 0; i < hook->last_wire_place[displayed_event_id][plane_id]; i++)
		{
			auto line = new TLine(plane_id / 16.0,
					      hook->wire_ids[displayed_event_id][plane_id][i]/float(WIRES_COUNT),
					      (plane_id+1) / 16.0,
					      hook->wire_ids[displayed_event_id][plane_id][i]/float(WIRES_COUNT));

			line->Draw();

			wires_lines.push_back(line);
		}
	}

	main_canvas->Show();
}

void	ShowEventsApp::Init()
{
	ifstream	file(geometry_filepath, ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry	geom(file);
	hook = new EvShowHook(geom);

	try
	{
		loadfile(data_filepath, *hook);
	}
	catch( int i )
	{
		if (i != 7)
		{
			throw i;
		}
	}

	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	title_text_widget = nullptr;

	next_button = new TButton("Next event", NEXT_EVENT_CMD, 0.75, 0.9, 0.95, 1);
	next_button->SetFillColor(42);
	next_button->Draw();

	prev_button = new TButton("Previous event", PREV_EVENT_CMD, 0.05, 0.9, 0.25, 1);
	prev_button->SetFillColor(18);
	prev_button->Draw();

	event_pad = new TPad("event_pad", "pad", 0.01, 0.01, 0.99, 0.88, 0, 0.02, 42);
	event_pad->Draw();
	event_pad->cd();

	displayed_event_id = 0;

	SetupTitle();

	PlotResults();
}

int	main( int argc, char* argv[] )
{
	ShowEventsApp	app("Momentum distribution", argc, argv);

	while(1) { app.Run(); };

	return 0;
}
