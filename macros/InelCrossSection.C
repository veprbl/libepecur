#define InelCrossSection_cxx
// The class definition in InelCrossSection.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("InelCrossSection.C")
// root> T->Process("InelCrossSection.C","some options")
// root> T->Process("InelCrossSection.C+")
//

#include <cmath>

#include "InelCrossSection.h"
#include <TH2.h>
#include <TStyle.h>
#include <TMath.h>

#include "kinematics_common.h"

void InelCrossSection::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

}

void InelCrossSection::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

	fNorm = new TH1F("fNorm", "normalization", BIN_COUNT, BIN_MIN, BIN_MAX);
	fNorm->Sumw2();
	GetOutputList()->Add(fNorm);
	fRawOutputLeft = new TH1F("fRawOutputLeft", "left", BIN_COUNT, BIN_MIN, BIN_MAX);
	fRawOutputLeft->Sumw2();
	GetOutputList()->Add(fRawOutputLeft);
	fRawOutputRight = new TH1F("fRawOutputRight", "Right", BIN_COUNT, BIN_MIN, BIN_MAX);
	fRawOutputRight->Sumw2();
	GetOutputList()->Add(fRawOutputRight);
}

Bool_t InelCrossSection::Process(Long64_t entry)
{
	// The Process() function is called for each entry in the tree (or possibly
	// keyed object in the case of PROOF) to be processed. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	// It can be passed to either InelCrossSection::GetEntry() or TBranch::GetEntry()
	// to read either all or the required parts of the data. When processing
	// keyed objects with PROOF, the object is already loaded and is available
	// via the fObject pointer.
	//
	// This function should contain the "body" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	//
	// Use fStatus to set the return value of TTree::Process().
	//
	// The return value is currently not used.

	GetEntry(entry);

	if (
		   (fabs((*t1X_c0)[0] + (*t1X_c1)[0] * 540) < 80)
		&& (sqr((*t2X_c0)[0] + (*t2X_c1)[0] * 990) + sqr((*t2Y_c0)[0] + (*t2Y_c1)[0] * 990) < 25)
		&& (min_cycle_efficiency > 0.15)
		)
	{
		const double W = calc_W(calc_beta(beam_momentum));
		if ((event_cause & 0x2) == 0x2)
		{
			fNorm->Fill(W);
		}

		if ((event_cause & 0x1) == 0x1)
		{
			if (
				   (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120)
				&& (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120)
				&& (abs(RL_x - RL_x) < 10) && (abs(RL_y - RL_y) < 10) && (abs(RL_z - RL_z) < 10)
				&& (theta_r > 25 * TMath::DegToRad()) && (theta_r < 70 * TMath::DegToRad())
				&& (theta_l > 25 * TMath::DegToRad()) && (theta_l < 70 * TMath::DegToRad())
				)
			{
				fRawOutputLeft->Fill(W);
			}
		}
	}

	return kTRUE;
}

void InelCrossSection::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.

}

void InelCrossSection::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.

	fRawOutputLeft->Add(fRawOutputRight);

	fOutputLeft = new TH1F("fOutputLeft", "Output in the left arm", BIN_COUNT, BIN_MIN, BIN_MAX);
	fOutputLeft->Sumw2();
	fOutputLeft->Divide(fRawOutputLeft, fNorm, 1, 100);
	GetOutputList()->Add(fOutputLeft);

	fOutputRight = new TH1F("fOutputRight", "Output in the right arm", BIN_COUNT, BIN_MIN, BIN_MAX);
	fOutputRight->Sumw2();
	fOutputRight->Divide(fRawOutputRight, fNorm, 1, 100);
	GetOutputList()->Add(fOutputRight);
}
