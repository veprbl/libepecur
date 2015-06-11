#!/usr/bin/env python2

for min_angle, max_angle in [(25, 35), (40, 50), (50, 60), (60, 70)]:
	print min_angle
	with open("r.inc", "w") as fp:
		fp.write(
	"""\
	                                   (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120)
	                                && (theta_r > %i*TMath::DegToRad()) && (theta_r < %i*TMath::DegToRad())
	                                && std::isnan(theta_l) // Require no track in the left arm\
	""" % (min_angle, max_angle))
	
	with open("l.inc", "w") as fp:
		fp.write(
	"""\
	                                   (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120)
	                                && (theta_l > %i*TMath::DegToRad()) && (theta_l < %i*TMath::DegToRad())
	                                && std::isnan(theta_r) // Require no track in the right arm\
	""" % (min_angle, max_angle))
	
	with open("finalization.inc", "w") as fp:
		fp.write("""\
	gStyle->SetPadRightMargin(0.12+0.9);
	gStyle->SetPadLeftMargin(0.13-0.9);
	gStyle->SetPaperSize(3,3);
	c_combine.cd();
	c_combine.SaveAs("final_plot_1track_%i_%i.tex");
	exit(0);
	""" % (min_angle, max_angle, min_angle, max_angle))
	import os
	os.system("root final_plot.C")
