//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Apr  9 19:15:28 2015 by ROOT version 6.02/05
// from TTree events/recognized tracks for prop chambersand triggered drift wires
// found on file: /home/veprbl/pass2_full/27041098.root
//////////////////////////////////////////////////////////

#ifndef InclCrossSection_h
#define InclCrossSection_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class InclCrossSection : public TSelector
{
public :
	TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

	// Declaration of leaf types
	UInt_t          event_cause;
	UInt_t          timestamp;
	UInt_t          t1X_track_count;
	vector<double>  *t1X_c0;
	vector<double>  *t1X_c1;
	vector<unsigned int> *t1X_hits_count;
	vector<unsigned int> *t1X_used_chambers_mask;
	UInt_t          t1Y_track_count;
	vector<double>  *t1Y_c0;
	vector<double>  *t1Y_c1;
	vector<unsigned int> *t1Y_hits_count;
	vector<unsigned int> *t1Y_used_chambers_mask;
	UInt_t          t2X_track_count;
	vector<double>  *t2X_c0;
	vector<double>  *t2X_c1;
	vector<unsigned int> *t2X_hits_count;
	vector<unsigned int> *t2X_used_chambers_mask;
	UInt_t          t2Y_track_count;
	vector<double>  *t2Y_c0;
	vector<double>  *t2Y_c1;
	vector<unsigned int> *t2Y_hits_count;
	vector<unsigned int> *t2Y_used_chambers_mask;
	UInt_t          t3X_track_count;
	vector<double>  *t3X_c0;
	vector<double>  *t3X_c1;
	vector<unsigned int> *t3X_hits_count;
	vector<unsigned int> *t3X_used_chambers_mask;
	UInt_t          t3Y_track_count;
	vector<double>  *t3Y_c0;
	vector<double>  *t3Y_c1;
	vector<unsigned int> *t3Y_hits_count;
	vector<unsigned int> *t3Y_used_chambers_mask;
	UInt_t          t4X_track_count;
	vector<double>  *t4X_c0;
	vector<double>  *t4X_c1;
	vector<unsigned int> *t4X_hits_count;
	vector<unsigned int> *t4X_used_chambers_mask;
	UInt_t          t4Y_track_count;
	vector<double>  *t4Y_c0;
	vector<double>  *t4Y_c1;
	vector<unsigned int> *t4Y_hits_count;
	vector<unsigned int> *t4Y_used_chambers_mask;
	Double_t        min_cycle_efficiency;
	Double_t        LR_LR_x;
	Double_t        LR_LR_y;
	Double_t        LR_LR_z;
	Double_t        RL_RL_x;
	Double_t        RL_RL_y;
	Double_t        RL_RL_z;
	Double_t        F2R_F2R_x;
	Double_t        F2R_F2R_y;
	Double_t        F2R_F2R_z;
	Double_t        F2L_F2L_x;
	Double_t        F2L_F2L_y;
	Double_t        F2L_F2L_z;
	Double_t        RF2_RF2_x;
	Double_t        RF2_RF2_y;
	Double_t        RF2_RF2_z;
	Double_t        LF2_LF2_x;
	Double_t        LF2_LF2_y;
	Double_t        LF2_LF2_z;
	Double_t        theta_l;
	Double_t        theta_r;
	Double_t        phi_l;
	Double_t        phi_r;
	Double_t        beam_momentum;
	Double_t        incident_momentum_l;
	Double_t        incident_momentum_r;

	// List of branches
	TBranch        *b_event_cause;   //!
	TBranch        *b_timestamp;   //!
	TBranch        *b_t1X_track_count;   //!
	TBranch        *b_t1X_c0;   //!
	TBranch        *b_t1X_c1;   //!
	TBranch        *b_t1X_hits_count;   //!
	TBranch        *b_t1X_used_chambers_mask;   //!
	TBranch        *b_t1Y_track_count;   //!
	TBranch        *b_t1Y_c0;   //!
	TBranch        *b_t1Y_c1;   //!
	TBranch        *b_t1Y_hits_count;   //!
	TBranch        *b_t1Y_used_chambers_mask;   //!
	TBranch        *b_t2X_track_count;   //!
	TBranch        *b_t2X_c0;   //!
	TBranch        *b_t2X_c1;   //!
	TBranch        *b_t2X_hits_count;   //!
	TBranch        *b_t2X_used_chambers_mask;   //!
	TBranch        *b_t2Y_track_count;   //!
	TBranch        *b_t2Y_c0;   //!
	TBranch        *b_t2Y_c1;   //!
	TBranch        *b_t2Y_hits_count;   //!
	TBranch        *b_t2Y_used_chambers_mask;   //!
	TBranch        *b_t3X_track_count;   //!
	TBranch        *b_t3X_c0;   //!
	TBranch        *b_t3X_c1;   //!
	TBranch        *b_t3X_hits_count;   //!
	TBranch        *b_t3X_used_chambers_mask;   //!
	TBranch        *b_t3Y_track_count;   //!
	TBranch        *b_t3Y_c0;   //!
	TBranch        *b_t3Y_c1;   //!
	TBranch        *b_t3Y_hits_count;   //!
	TBranch        *b_t3Y_used_chambers_mask;   //!
	TBranch        *b_t4X_track_count;   //!
	TBranch        *b_t4X_c0;   //!
	TBranch        *b_t4X_c1;   //!
	TBranch        *b_t4X_hits_count;   //!
	TBranch        *b_t4X_used_chambers_mask;   //!
	TBranch        *b_t4Y_track_count;   //!
	TBranch        *b_t4Y_c0;   //!
	TBranch        *b_t4Y_c1;   //!
	TBranch        *b_t4Y_hits_count;   //!
	TBranch        *b_t4Y_used_chambers_mask;   //!
	TBranch        *b_min_cycle_efficiency;   //!
	TBranch        *b_LR;   //!
	TBranch        *b_RL;   //!
	TBranch        *b_F2R;   //!
	TBranch        *b_F2L;   //!
	TBranch        *b_RF2;   //!
	TBranch        *b_LF2;   //!
	TBranch        *b_theta_l;   //!
	TBranch        *b_theta_r;   //!
	TBranch        *b_phi_l;   //!
	TBranch        *b_phi_r;   //!
	TBranch        *b_beam_momentum;   //!
	TBranch        *b_incident_momentum_l;   //!
	TBranch        *b_incident_momentum_r;   //!

	InclCrossSection(TTree * /*tree*/ =0) : fChain(0) { }
	virtual ~InclCrossSection() { }
	virtual Int_t   Version() const
	{
		return 2;
	}
	virtual void    Begin(TTree *tree);
	virtual void    SlaveBegin(TTree *tree);
	virtual void    Init(TTree *tree);
	virtual Bool_t  Notify();
	virtual Bool_t  Process(Long64_t entry);
	virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0)
	{
		return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
	}
	virtual void    SetOption(const char *option)
	{
		fOption = option;
	}
	virtual void    SetObject(TObject *obj)
	{
		fObject = obj;
	}
	virtual void    SetInputList(TList *input)
	{
		fInput = input;
	}
	virtual TList  *GetOutputList() const
	{
		return fOutput;
	}
	virtual void    SlaveTerminate();
	virtual void    Terminate();

	ClassDef(InclCrossSection,0);
};

#endif

#ifdef InclCrossSection_cxx
void InclCrossSection::Init(TTree *tree)
{
	// The Init() function is called when the selector needs to initialize
	// a new tree or chain. Typically here the branch addresses and branch
	// pointers of the tree will be set.
	// It is normally not necessary to make changes to the generated
	// code, but the routine can be extended by the user if needed.
	// Init() will be called many times when running on PROOF
	// (once per file to be processed).

	// Set object pointer
	t1X_c0 = 0;
	t1X_c1 = 0;
	t1X_hits_count = 0;
	t1X_used_chambers_mask = 0;
	t1Y_c0 = 0;
	t1Y_c1 = 0;
	t1Y_hits_count = 0;
	t1Y_used_chambers_mask = 0;
	t2X_c0 = 0;
	t2X_c1 = 0;
	t2X_hits_count = 0;
	t2X_used_chambers_mask = 0;
	t2Y_c0 = 0;
	t2Y_c1 = 0;
	t2Y_hits_count = 0;
	t2Y_used_chambers_mask = 0;
	t3X_c0 = 0;
	t3X_c1 = 0;
	t3X_hits_count = 0;
	t3X_used_chambers_mask = 0;
	t3Y_c0 = 0;
	t3Y_c1 = 0;
	t3Y_hits_count = 0;
	t3Y_used_chambers_mask = 0;
	t4X_c0 = 0;
	t4X_c1 = 0;
	t4X_hits_count = 0;
	t4X_used_chambers_mask = 0;
	t4Y_c0 = 0;
	t4Y_c1 = 0;
	t4Y_hits_count = 0;
	t4Y_used_chambers_mask = 0;
	// Set branch addresses and branch pointers
	if (!tree) return;
	fChain = tree;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("event_cause", &event_cause, &b_event_cause);
	fChain->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
	fChain->SetBranchAddress("t1X_track_count", &t1X_track_count, &b_t1X_track_count);
	fChain->SetBranchAddress("t1X_c0", &t1X_c0, &b_t1X_c0);
	fChain->SetBranchAddress("t1X_c1", &t1X_c1, &b_t1X_c1);
	fChain->SetBranchAddress("t1X_hits_count", &t1X_hits_count, &b_t1X_hits_count);
	fChain->SetBranchAddress("t1X_used_chambers_mask", &t1X_used_chambers_mask, &b_t1X_used_chambers_mask);
	fChain->SetBranchAddress("t1Y_track_count", &t1Y_track_count, &b_t1Y_track_count);
	fChain->SetBranchAddress("t1Y_c0", &t1Y_c0, &b_t1Y_c0);
	fChain->SetBranchAddress("t1Y_c1", &t1Y_c1, &b_t1Y_c1);
	fChain->SetBranchAddress("t1Y_hits_count", &t1Y_hits_count, &b_t1Y_hits_count);
	fChain->SetBranchAddress("t1Y_used_chambers_mask", &t1Y_used_chambers_mask, &b_t1Y_used_chambers_mask);
	fChain->SetBranchAddress("t2X_track_count", &t2X_track_count, &b_t2X_track_count);
	fChain->SetBranchAddress("t2X_c0", &t2X_c0, &b_t2X_c0);
	fChain->SetBranchAddress("t2X_c1", &t2X_c1, &b_t2X_c1);
	fChain->SetBranchAddress("t2X_hits_count", &t2X_hits_count, &b_t2X_hits_count);
	fChain->SetBranchAddress("t2X_used_chambers_mask", &t2X_used_chambers_mask, &b_t2X_used_chambers_mask);
	fChain->SetBranchAddress("t2Y_track_count", &t2Y_track_count, &b_t2Y_track_count);
	fChain->SetBranchAddress("t2Y_c0", &t2Y_c0, &b_t2Y_c0);
	fChain->SetBranchAddress("t2Y_c1", &t2Y_c1, &b_t2Y_c1);
	fChain->SetBranchAddress("t2Y_hits_count", &t2Y_hits_count, &b_t2Y_hits_count);
	fChain->SetBranchAddress("t2Y_used_chambers_mask", &t2Y_used_chambers_mask, &b_t2Y_used_chambers_mask);
	fChain->SetBranchAddress("t3X_track_count", &t3X_track_count, &b_t3X_track_count);
	fChain->SetBranchAddress("t3X_c0", &t3X_c0, &b_t3X_c0);
	fChain->SetBranchAddress("t3X_c1", &t3X_c1, &b_t3X_c1);
	fChain->SetBranchAddress("t3X_hits_count", &t3X_hits_count, &b_t3X_hits_count);
	fChain->SetBranchAddress("t3X_used_chambers_mask", &t3X_used_chambers_mask, &b_t3X_used_chambers_mask);
	fChain->SetBranchAddress("t3Y_track_count", &t3Y_track_count, &b_t3Y_track_count);
	fChain->SetBranchAddress("t3Y_c0", &t3Y_c0, &b_t3Y_c0);
	fChain->SetBranchAddress("t3Y_c1", &t3Y_c1, &b_t3Y_c1);
	fChain->SetBranchAddress("t3Y_hits_count", &t3Y_hits_count, &b_t3Y_hits_count);
	fChain->SetBranchAddress("t3Y_used_chambers_mask", &t3Y_used_chambers_mask, &b_t3Y_used_chambers_mask);
	fChain->SetBranchAddress("t4X_track_count", &t4X_track_count, &b_t4X_track_count);
	fChain->SetBranchAddress("t4X_c0", &t4X_c0, &b_t4X_c0);
	fChain->SetBranchAddress("t4X_c1", &t4X_c1, &b_t4X_c1);
	fChain->SetBranchAddress("t4X_hits_count", &t4X_hits_count, &b_t4X_hits_count);
	fChain->SetBranchAddress("t4X_used_chambers_mask", &t4X_used_chambers_mask, &b_t4X_used_chambers_mask);
	fChain->SetBranchAddress("t4Y_track_count", &t4Y_track_count, &b_t4Y_track_count);
	fChain->SetBranchAddress("t4Y_c0", &t4Y_c0, &b_t4Y_c0);
	fChain->SetBranchAddress("t4Y_c1", &t4Y_c1, &b_t4Y_c1);
	fChain->SetBranchAddress("t4Y_hits_count", &t4Y_hits_count, &b_t4Y_hits_count);
	fChain->SetBranchAddress("t4Y_used_chambers_mask", &t4Y_used_chambers_mask, &b_t4Y_used_chambers_mask);
	fChain->SetBranchAddress("min_cycle_efficiency", &min_cycle_efficiency, &b_min_cycle_efficiency);
	fChain->SetBranchAddress("LR", &LR_LR_x, &b_LR);
	fChain->SetBranchAddress("RL", &RL_RL_x, &b_RL);
	fChain->SetBranchAddress("F2R", &F2R_F2R_x, &b_F2R);
	fChain->SetBranchAddress("F2L", &F2L_F2L_x, &b_F2L);
	fChain->SetBranchAddress("RF2", &RF2_RF2_x, &b_RF2);
	fChain->SetBranchAddress("LF2", &LF2_LF2_x, &b_LF2);
	fChain->SetBranchAddress("theta_l", &theta_l, &b_theta_l);
	fChain->SetBranchAddress("theta_r", &theta_r, &b_theta_r);
	fChain->SetBranchAddress("phi_l", &phi_l, &b_phi_l);
	fChain->SetBranchAddress("phi_r", &phi_r, &b_phi_r);
	fChain->SetBranchAddress("beam_momentum", &beam_momentum, &b_beam_momentum);
	fChain->SetBranchAddress("incident_momentum_l", &incident_momentum_l, &b_incident_momentum_l);
	fChain->SetBranchAddress("incident_momentum_r", &incident_momentum_r, &b_incident_momentum_r);
}

Bool_t InclCrossSection::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.

	return kTRUE;
}

#endif // #ifdef InclCrossSection_cxx