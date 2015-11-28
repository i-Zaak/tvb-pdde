#include "model.h"
#include "assert.h"
generic_2D_oscillator::generic_2D_oscillator( double tau, double a,
								double b,
								double c,
								double I,
								double d,
								double e,
								double f,
								double g,
								double alpha,
								double beta,
								double gamma)
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
}




void generic_2D_oscillator::operator()(	const local_state_type &phi, 
										const local_coupling_type &coupling, 
										local_state_type &dphidt)
{

	assert(phi.size() == dphidt.size() == coupling.size() == 2); 

	double V = phi[0];
	double W = phi[1];

	double c_0 = coupling[0];

	double lc_0 = coupling[1] * V; // local_coupling

	dphidt[0] = d * tau * (alpha * W - f * V*V*V + e * V*V + g * V + gamma * I + gamma *c_0 + lc_0);
	dphidt[1] = d * (a + b * V + c * V*V - beta * W) / tau;


}
