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




void generic_2d_oscillator::operator()(	const local_state_type &phi, 
										const local_coupling_type &coupling, 
										local_state_type &df,
										local_state_type &dg)
{

	assert( phi.size() == df.size() && 
			phi.size() == dg.size() && 
			phi.size() == coupling.size() && 
			phi.size() == 2); 

	double V = phi[0];
	double W = phi[1];

	double c_0 = coupling[0];

	df[0] = d * tau * (alpha * W - f * V*V*V + e * V*V + g * V + gamma * I + gamma *c_0 );
	df[1] = d * (a + b * V + c * V*V - beta * W) / tau;


}
