#include <TChain.h>
#include <TH1F.h>

void	final_plot()
{
	TChain	*c = new TChain("events");
	c->Add(Form("%s/*.root", gSystem->Getenv("EPECUR_PROCESS_OUTDIR")));
	TH1F	*or = new TH1F("or", "output", (1250-820)/1, 820, 1250);
	c->Draw("incident_momentum_r >> or",
"((((event_cause & 0x1) == 0x1) && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120) && (theta_r > 0) && (theta_l != theta_l) && (efficiency_r > 0.7)) ? 1 : 0) / efficiency_r"
		);
	TH1F	*ol = new TH1F("ol", "output", (1250-820)/1, 820, 1250);
	c->Draw("incident_momentum_l >> ol",
"((((event_cause & 0x1) == 0x1) && (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120) && (theta_l > 0) && (theta_r != theta_r) && (efficiency_l > 0.7)) ? 1 : 0) / efficiency_l"
		);
	TH1F	*o = new TH1F(*or + *ol);
	o->SetName("o");
	TH1F	*n = new TH1F("n", "normalization", (1250-820)/1, 820, 1250);
	c->Draw("beam_momentum >> n", "(event_cause & 0x2) == 0x2");
	TH1F	*r = new TH1F("r", "result", (1250-820)/1, 820, 1250);
	r->Divide(o, n);
	r->Draw();
}
