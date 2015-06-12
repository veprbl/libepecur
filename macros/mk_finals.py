#!/usr/bin/env python2

for min_angle, max_angle in [(40, 50), (50, 60), (60, 70), (70, 80), (80, 90), (90, 100), (100, 110), (110, 120)]:
	print min_angle
	with open("r.inc", "w") as fp:
		fp.write(
	"""\
	                                   (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120)
	                                && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120)
					&& (fabs(phi_r - phi_l + 3.14) < 0.1)
					&& (!std::isnan(theta_l))
					&& (!std::isnan(theta_r))
					&& (fabs(calc_theta_cm_from_theta_p_lab(beam_momentum, theta_l) - calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_r) + 2 * TMath::DegToRad()) < 3 * TMath::DegToRad())
	                                && (calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_r) > %i*TMath::DegToRad()) && (calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_r) < %i*TMath::DegToRad())
	""" % (min_angle, max_angle))
	
	with open("l.inc", "w") as fp:
		fp.write(
	"""\
	                                   (F2L_x > -200) && (F2L_x < 120) && (LF2_x > -200) && (LF2_x < 120)
	                                && (F2R_x > -200) && (F2R_x < 120) && (RF2_x > -200) && (RF2_x < 120)
					&& (fabs(phi_r - phi_l + 3.14) < 0.1)
					&& (!std::isnan(theta_l))
					&& (!std::isnan(theta_r))
					&& (fabs(calc_theta_cm_from_theta_p_lab(beam_momentum, theta_r) - calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_l) + 2 * TMath::DegToRad()) < 3 * TMath::DegToRad())
	                                && (calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_l) > %i*TMath::DegToRad()) && (calc_theta_cm_from_theta_pi_lab(beam_momentum, theta_l) < %i*TMath::DegToRad())
	""" % (min_angle, max_angle))
	
	with open("finalization.inc", "w") as fp:
		fp.write("""\
	gStyle->SetPadRightMargin(0.12+0.9);
	gStyle->SetPadLeftMargin(0.13-0.9);
	gStyle->SetPaperSize(3,3);
	c_combine.cd();
	c_combine.SaveAs("final_plot_2track_elastic_%i_%i.tex");
	exit(0);
	""" % (min_angle, max_angle))
	import os
	os.system("root final_plot.C")
	import time
	time.sleep(10)
