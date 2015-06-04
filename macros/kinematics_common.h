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
