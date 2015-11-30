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
		population_model model; //could be private
		model_coupling coupling; //could be private
		global_connectivity_type connectivity; //could be private
		global_history_type history; //could be private
		double dt;
		unsigned long n_nodes;
		virtual void scheme(local_state_type &new_state)=0; // the actual integration scheme
		void step();
		void dfun_eval(	unsigned int node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &dphidt);

	public:
		integrator(	population_model model,
					model_coupling coupling,
					global_connectivity_type connectivity,
					global_history_type initial_conditions,
					unsigned long n_nodes,
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
								unsigned long n_nodes,
								double dt
							):integrator(
								population_model model,
								model_coupling coupling,
								global_connectivity_type connectivity,
								global_history_type initial_conditions,
								unsigned long n_nodes,
								double dt
							){};
};

