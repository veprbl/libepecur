TCanvas c("c", "", 300, 300);

void drift_pickups()
{
	TFile *f = new TFile("/tmp/25041078.root");
	TTree *events = (TTree*)f->Get("events");
	gStyle->SetPaperSize(5,5);
	TH1F *h = new TH1F("h", ";\\left|\\Delta T_{\\text{дрейфа}}\\right|, 1;\\frac{dN}{d \\Delta T_{\\text{дрейфа}}}", 30, 0., 30.);
	h->GetYaxis()->SetNdivisions(3);
	events->Draw("abs(d3Y1_time[0]-d3Y2_time[0]) >> h", "(abs(abs(d3Y1_wire_pos[0] - d3Y2_wire_pos[0]) - 1.) < 0.1) && (d3Y1_num_wires >= 1) && (d3Y2_num_wires >= 1)");
	c.SaveAs("d3Y1_pickups.tex");
	events->Draw("abs(d3Y1_time[0]-d3Y1_time[1]) >> h", "(abs(abs(d3Y1_wire_pos[0] - d3Y1_wire_pos[1]) - 2.) < 0.1) && (d3Y1_num_wires >= 2)");
	c.SaveAs("d3Y1_pickups_adj.tex");
	events->Draw("abs(d3Y1_time[0]-d3Y1_time[1]) >> h", "(abs(d3Y1_wire_pos[0] - d3Y1_wire_pos[1]) > 2.1) && (d3Y1_num_wires >= 2)");
	c.SaveAs("d3Y1_pickups_adj_sep.tex");
}
