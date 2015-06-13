TCanvas c("c", "", 300, 300);

void first_focus()
{
        TFile *f = new TFile("~/pass1_full/20041008.root");
        TTree *events = (TTree*)f->Get("events");
        gStyle->SetPaperSize(5,5);

	//TH2F *h1 = new TH2F("h1", ";X_{\\text{1}^{\\text{й}}\\text{ фокус}}:Y_{\\text{1}^{\\text{й}}\\text{ фокус}}", 200, -100, 100, 20, -20, 20);
	//events->Draw("(t1Y_c0+t1Y_c1*540.0):(t1X_c0+t1X_c1*540.0) >> h1", "((event_cause & 0x1) == 0x1)", "COLZ");

	TH1F *h1 = new TH1F("h1", ";X_{\\text{1}^{\\text{й}}\\text{ фокус}}", 200, -100, 100);
	events->Draw("(t1X_c0+t1X_c1*540.0) >> h1", "((event_cause & 0x1) == 0x1)");
	c.SaveAs("x_1f_T1.tex");
	events->Draw("(t1X_c0+t1X_c1*540.0) >> h1", "((event_cause & 0x2) == 0x2)");
	c.SaveAs("x_1f_T2.tex");

        TH2F *h = new TH2F("h", ";X_{\\text{2}^{\\text{й}}\\text{ фокус}}\\text{, мм};Y_{\\text{2}^{\\text{й}}\\text{ фокус}}\\text{, мм}", 40, -20, 20, 40, -20, 20);
        events->Draw("(t2Y_c0+t2Y_c1*990):(t2X_c0+t2X_c1*990) >> h", "", "COLZ");
        c.SaveAs("xy_2f.tex");
}
