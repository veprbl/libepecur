#ifndef __SHOWEVENTSAPP_HPP
#define __SHOWEVENTSAPP_HPP

#include <vector>

#include <TApplication.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TPaveText.h>

#include "EvShowHook.hpp"

class	ShowEventsApp: public TApplication
{
private:
	TCanvas*	main_canvas;
	TPaveText*	title_text_widget;
	string		title_text;
	TPad*		event_pad;
	vector<TLine*>	wires_lines;
	TButton*	next_button;
	TButton*	prev_button;
	EvShowHook*	hook;
	uint		displayed_event_id;

	void	ParseCommandLine();
	void	SetupButtonColors();
	void	SetupTitle();

public:
	ShowEventsApp( const char* app_name, int &argc, char* argv[] );
	~ShowEventsApp();

	string	data_filepath;
	string	geometry_filepath;

	void	Init();
	Long_t	ProcessLine( const char* line, Bool_t sync = kFALSE, Int_t* error = 0 );
	void	PlotResults();
};

#endif
