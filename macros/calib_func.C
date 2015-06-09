TCanvas c("c", "", 300, 300);

void calib_func()
{
	TFile f("~/pass1_full/20041008.root");
	TTree *events = (TTree*)f.Get("events");
	gStyle->SetPaperSize(5,5);
	events->Draw("d3Y1_time");
	c.SaveAs("d3Y1_time.tex");
	events->Draw("d3Y1_time", "d3Y1_num_wires == 1");
	c.SaveAs("d3Y1_time_1wire.tex");
	events->Draw("d3Y1_time", "(d3Y1_num_wires == 2) && abs(d3Y1_wire_pos[0] - d3Y1_wire_pos[1]) == 2");
	c.SaveAs("d3Y1_time_adj.tex");
}
