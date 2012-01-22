#ifndef __MOMENTUMDISPERSIONAPP_HPP
#define __MOMENTUMDISPERSIONAPP_HPP

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
	void	PlotResults( uint planes[][200] );
};

#endif
