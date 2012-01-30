#ifndef __SHOWEVENTSAPP_HPP
#define __SHOWEVENTSAPP_HPP

#include <vector>

#include "AxialShiftHook.hpp"

class	AxialShiftApp: public TApplication
{
private:

	TCanvas*	main_canvas;
	TPad*		hist_pad;
	TPaveText*	title_text_widget;
	unordered_map< chamber_id_t, TH1I* >	shift_hist;
	AxialShiftHook*	hook;

	void	ParseCommandLine();
	void	init_hists( Geometry &geom );

public:

	AxialShiftApp( const char* app_name, int &argc, char* argv[] );
	~AxialShiftApp();

	string	data_filepath;
	string	geometry_filepath;

	void	Init();
};

#endif
