#include "model.h"
#include "assert.h"
generic_2d_oscillator::generic_2d_oscillator( double tau, double a,
								double b,
								double c,
								double I,
								double d,
								double e,
								double f,
								double g,
								double alpha,
								double beta,
								double gamma,
								double sigma)
{
	this->tau = tau;
	this->a = a;
	this->b = b;
	this->c = c;
	this->I = I;
	this->d = d;
	this->e = e;
	this->f = f;
	this->g = g;
	this->alpha = alpha;
	this->beta = beta;
	this->gamma = gamma;
	this->sigma = sigma;
}




void generic_2d_oscillator::operator()(	const local_state_type &phi, 
										const local_coupling_type &coupling, 
										local_state_type &df,
										local_state_type &dg)
{
	assert( phi.size() == df.size() && 
			phi.size() == dg.size() && 
			phi.size() == 2 &&
			coupling.size() == 2); // :(

	// TODO DEBUG macro for speed...
	for (std::size_t i = 0; i < coupling.size(); i++) {
		assert(phi.size() == coupling[i].size());
	}

	double V = phi[0];
	double W = phi[1];

	double sc_0 = coupling[0][0]; //surface
	double rc_0 = coupling[1][0]; //regional

	df[0] = d * tau * (alpha * W - f * V*V*V + e * V*V + g * V + gamma * I + gamma * rc_0 + sc_0 * V); //TODO is this correct?
	df[1] = d * (a + b * V + c * V*V - beta * W) / tau;

	dg[0] = sigma;
	dg[1] = sigma;


}
