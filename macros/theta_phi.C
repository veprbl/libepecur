TCanvas c("c", "", 600, 300);

void theta_phi()
{
        TFile *f = new TFile("~/pass2_full/20041008.root");
        TTree *events = (TTree*)f->Get("events");
gROOT->SetStyle("BABAR");
        gStyle->SetPaperSize(10,10);
	//TH2F *h1 = new TH2F("h1", ";X_{\\text{1}^{\\text{й}}\\text{ фокус}}:Y_{\\text{1}^{\\text{й}}\\text{ фокус}}", 200, -100, 100, 20, -20, 20);
	//events->Draw("(t1Y_c0+t1Y_c1*540.0):(t1X_c0+t1X_c1*540.0) >> h1", "((event_cause & 0x1) == 0x1)", "COLZ");

	c.Divide(2);

	c.cd(1);
        events->Draw("(phi_l/3.14*180):(theta_l/3.14*180) >> corr(40, 0., 120, 40, 0., 180)", "", "COLZ");
        TH2F *corr = (TH2F*)gDirectory->Get("corr");
        corr->GetXaxis()->SetTitle("\\Theta_l");
        corr->GetYaxis()->SetTitle("\\varphi_l");
	c.cd(2);
        events->Draw("(phi_r/3.14*180):(theta_r/3.14*180) >> corr2(40, 0., 120, 40, -180, 0.)", "", "COLZ");
        corr = (TH2F*)gDirectory->Get("corr2");
        corr->GetXaxis()->SetTitle("\\Theta_r");
        corr->GetYaxis()->SetTitle("\\varphi_r");

        c.SaveAs("theta_phi.tex");
}
