#ifndef __MOMENTUMDISPERSIONAPP_HPP
#define __MOMENTUMDISPERSIONAPP_HPP

#include <TApplication.h>
#include <TCanvas.h>
#include <TH1I.h>

#include "EvSumHook.hpp"

class	MomentumDistributionApp: public TApplication
{
private:
	TCanvas*	main_canvas;
	vector<TH1I*>	th;

	void	ParseCommandLine();

public:
	MomentumDistributionApp( const char* app_name, int &argc, char* argv[] );
	~MomentumDistributionApp();

	string	data_filepath;
	string	geometry_filepath;

	void	Init();
	void	PlotResults( uint chambers[][WIRES_COUNT] );
};

#endif
