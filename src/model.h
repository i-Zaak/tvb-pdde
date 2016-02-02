#ifndef MODEL_H
#define MODEL_H

#include "common.h"
/**
 Provides evaluation of the right-hand side of the model for single population. Namely the functions `f` and `g` from the Langevin form of the equations. Also holds the constants. Sizes of the arrays are fixed for given model. 
 `n_vars`		number of state variables
 `phi`			local set of state variables
 `coupling` 	contributions from other populations: can have multiple
 				components (coupling variables)
 `df`			result of the same size as `phi`: drift
 `dg`			result of the same size as `phi`: diffusion


 */
class population_model
{
	public:
		virtual unsigned int n_vars()=0;
		virtual void operator()(	const local_state_type &phi, 
									const local_coupling_type &coupling, 
									local_state_type &df,
									local_state_type &dg)=0;
};

class generic_2d_oscillator : public population_model
{
	private:
		// TODO getters, setters
		double tau;
		double a;
		double b;
		double c;
		double I;
		double d;
		double e;
		double f;
		double g;
		double alpha;
		double beta;
		double gamma;
		double sigma; // coefficient for noise driven term
	public:
		inline unsigned int n_vars() {
			return 2;
		}
		generic_2d_oscillator(	double tau = 1.0,
								double a = -2.0,
								double b = -10.0,
								double c = 0.0,
								double I = 0.0,
								double d = 0.02,
								double e = 3.0,
								double f = 1.0,
								double g = 0.0,
								double alpha = 1.0,
								double beta = 1.0,
								double gamma = 1.0,
								double sigma = 0.001 );//TODO inline?
		void operator()(	const local_state_type &phi, 
							const local_coupling_type &coupling, 
							local_state_type &df,
							local_state_type &dg);
};

#endif
