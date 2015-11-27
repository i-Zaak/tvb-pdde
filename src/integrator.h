#include "common.h"
/**
 Performs the main integration loop for every node:
  * query history
  * evaluate coupling
  * perform integration step
  * update history

 Also setups, holds and tears down the integration data structures. 
 */
class integrator 
{
	protected:
		model_coupling coupling;
		integration_scheme scheme;
		
	public:
		integrator(	population_model model,
					model_coupling coupling,
					integration_scheme scheme);
};

