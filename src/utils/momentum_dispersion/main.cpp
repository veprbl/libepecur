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

namespace po = boost::program_options;

using namespace std;

const int	MAX_PLANE_ID = 16;

Geometry	*g;
uint	planes[MAX_PLANE_ID][200];

void	h( const char* begin, const char* end, uint16_t dev_id, void* data )
{
	plane_id_t	plane_id = g->get_device_plane(dev_id);

	for(auto pos = begin; pos < end; pos++)
	{
		int	wire_pos = g->get_wire_pos(dev_id, *pos);

		planes[plane_id][wire_pos + 100] += 1;
	}
}

class	MomentumDistributionApp: public TApplication
{
private:
	TCanvas*	main_canvas;
	vector<TH1I*>	th;

	void	ParseCommandLine( int argc, char* argv[] );

public:
	MomentumDistributionApp(
		const char* appClassName, Int_t* argc, char** argv, void* options = 0, Int_t numOptions = 0
		);
	~MomentumDistributionApp();

	string	data_filepath;
	string	geometry_filepath;

	void	Init();
	void	PlotResults();
};

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

void	MomentumDistributionApp::PlotResults()
{
	main_canvas = new TCanvas("main_canvas", ApplicationName(), 200, 10, 1000, 500);
	main_canvas->Connect("Closed()", "TApplication", this, "Terminate()");

	int	rows_count = floor(sqrt(MAX_PLANE_ID));
	int	cols_count = ceil(MAX_PLANE_ID / rows_count);

	main_canvas->Divide(rows_count, cols_count);

	for(int plane_id = 0; plane_id < MAX_PLANE_ID; plane_id++)
	{
		string	name = boost::lexical_cast<string>(plane_id);
		TH1I*	hist = new TH1I(name.c_str(), "qwe", 200, -100, 100);

		main_canvas->cd(plane_id + 1);

		for(int wire_id = 0; wire_id < 200; wire_id++)
		{
			hist->Fill(wire_id - 100, planes[plane_id][wire_id]);
		}

		hist->Draw();
		th.push_back(hist);
	}

	main_canvas->Show();
}

void	MomentumDistributionApp::Init()
{
	load_hooks_t	hooks;

	hooks.prop_handler = h;

	ifstream	file(geometry_filepath, ios::in);

	if (!file.is_open())
	{
		throw "Couldn't open geometry file!";
	}

	Geometry a(file);
	g = &a;

	loadfile(data_filepath, hooks);

	PlotResults();
}

int	main( int argc, char* argv[] )
{
	MomentumDistributionApp	app("Momentum distribution", &argc, argv);

	while(1) { app.Run(); };

	return 0;
}
