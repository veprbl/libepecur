const double M_p = 938.272046; // MeV
const double M_pi = 139.57018; // MeV

static double sqr(double x)
{
        return x*x;
}

static double calc_gamma(double beta)
{
        return 1/sqrt(1 - sqr(beta));
}

/**
 * Calculate beta corresponding to lorentz transformation from laboratory to
 * the center of inertia system.
 */
static double calc_beta(double beam_momentum)
{
        return beam_momentum / (M_p + sqrt(sqr(M_pi) + sqr(beam_momentum)));
}

/**
 * Calculate momentum in center of inertia system.
 */
static double calc_p(double beta)
{
        return M_p * beta * calc_gamma(beta);
}

/**
 * Calculate energy in center of inertia system.
 */
static double calc_W(double beta)
{
	double p = calc_p(beta);

	return sqrt(sqr(M_pi) + sqr(p)) + sqrt(sqr(M_p) + sqr(p));
}

/**
 * Calculate theta_cm from theta_pi in laboratory system
 */
double calc_theta_cm_from_theta_pi_lab(double beam_momentum, double theta_pi_lab)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);
	const double t = tan(theta_pi_lab);

	return
		acos(
			-gamma * abs(t) * beta
			/ p
			* sqrt(
				(sqr(M_pi) + sqr(p))
				/
				(1 + sqr(gamma * t))
			)
		)
		- atan(1 / (gamma * t));
}

/**
 * Calculate theta_cm from theta_p in laboratory system
 */
double calc_theta_cm_from_theta_p_lab(double beam_momentum, double theta_p_lab)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);
	const double t = tan(theta_p_lab);

	if (theta_p_lab > M_PI_2)
	{
		return NAN;
	}

	return
		acos(
			gamma * abs(t) * beta
			/ p
			* sqrt(
				(sqr(M_p) + sqr(p))
				/
				(1 + sqr(gamma * t))
			)
		)
		- atan(1 / (gamma * -t));
}

/**
 * Calculate theta_pi in laboratory system from theta_cm
 */
double calc_theta_lab_pi(double beam_momentum, double theta_cm)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);

	return
		atan2(
			p * sin(theta_cm)
			,
			gamma * (p * cos(theta_cm) + beta * sqrt(sqr(M_pi) + sqr(p)))
		);
}

/**
 * Calculate theta_p in laboratory ststem from theta_cm
 */
double calc_theta_lab_p(double beam_momentum, double theta_cm)
{
	const double beta = calc_beta(beam_momentum);
	const double gamma = calc_gamma(beta);
	const double p = calc_p(beta);

	return
		atan2(
			p * sin(theta_cm)
			,
			gamma * (-p * cos(theta_cm) + beta * sqrt(sqr(M_p) + sqr(p)))
		);
}
