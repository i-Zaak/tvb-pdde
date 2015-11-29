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
		population_model model;
		model_coupling coupling;
		global_connectivity_type connectivity;
		global_history_type history;
		double dt;
		virtual void scheme(local_state_type &new_state)=0; // the actual integration scheme
		void step();
	public:
		integrator(	population_model model,
					model_coupling coupling,
					global_connectivity_type connectivity,
					global_history_type initial_conditions,
					double dt);
		void operator(unsigned int n_steps);
};

class euler_deterministic:integrator
{
	private:
		void scheme()
	public:
		euler_deterministic(	population_model model,
								model_coupling coupling,
								global_connectivity_type connectivity,
								global_history_type initial_conditions,
								double dt
							):integrator(
								population_model model,
								model_coupling coupling,
								global_connectivity_type connectivity,
								global_history_type initial_conditions,
								double dt
							){};
};

