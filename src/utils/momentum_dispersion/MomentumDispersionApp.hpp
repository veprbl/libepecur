#ifndef __MOMENTUMDISPERSIONAPP_HPP
#define __MOMENTUMDISPERSIONAPP_HPP

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
	void	PlotResults( uint planes[][200] );
};

#endif
