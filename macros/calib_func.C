TCanvas c("c", "", 300, 300);

void calib_func()
{
	TFile *f = new TFile("~/pass1_full/20041008.root");
	TTree *events = (TTree*)f->Get("events");
	gStyle->SetPaperSize(8,8);
	TH1F *h = new TH1F("h", ";T_{\\text{дрейфа}}, 1;\\frac{dN}{d T_{\\text{дрейфа}}}", 400, 0, 400.);
	h->GetYaxis()->SetNdivisions(3);
	events->Draw("d3Y1_time >> h");
	c.SaveAs("d3Y1_time.tex");
	events->Draw("d3Y1_time >> h", "d3Y1_num_wires == 1");
	c.SaveAs("d3Y1_time_1wire.tex");
	events->Draw("d3Y1_time >> h", "(d3Y1_num_wires == 2) && abs(d3Y1_wire_pos[0] - d3Y1_wire_pos[1]) == 2");
	c.SaveAs("d3Y1_time_adj.tex");
	events->Draw("d3Y1_time >> h", "(d3Y1_num_wires == 2) && abs(d3Y1_wire_pos[0] - d3Y1_wire_pos[1]) >= 4");
	c.SaveAs("d3Y1_time_sep.tex");
	Int_t xfirst  = h->GetXaxis()->GetFirst();
	Int_t xlast   = h->GetXaxis()->GetLast();
	for(Int_t bin = xfirst + 1; bin <= xlast; bin++)
	{
		h->SetBinContent(bin, h->GetBinContent(bin) + h->GetBinContent(bin - 1));
	}
	h->Scale(8.5 / h->GetBinContent(xlast));
	h->GetYaxis()->SetTitle("d\\text{, мм}");
	h->GetYaxis()->SetNdivisions();
	c.SaveAs("d3Y1_calib_sep.tex");
}
