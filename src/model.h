#include "common.h"
/**
 Provides `dfun` function to evaluate the right-hand side of the model for
 single population. Also holds the constants. Sizes of the arrays are fixed 
 for given model. 
 `n_vars`		number of state variables
 `phi`			local set of state variables
 `coupling` 	contributions from other populations: can have multiple
 				components (coupling variables)
 `dphidt`		result of the same size as `phi`


 */
class population_model
{
	public:
		virtual unsigned int n_vars()=0;
		virtual void operator()(	const local_state_type &phi, 
									const local_coupling_type &coupling, 
									local_state_type &dphidt)=0;
};

class generic_oscillator:population_model
{
	private:
		double tau = 1.0;
		double a = -2.0;
		double b = -10.0;
		double c = 0.0;
		double I = 0.0;
		double d = 0.02;
		double e = 3.0;
		double f = 1.0;
		double g = 0.0;
		double alpha = 1.0;
		double beta = 1.0;
		double gamma = 1.0;
	public:
		inline unsigned int n_vars() {
			return 2;
		}
		void operator()(	const local_state_type &phi, 
							const local_coupling_type &coupling, 
							local_state_type &dphidt);
}
