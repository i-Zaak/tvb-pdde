#ifndef INTEGRATOR_H
#define INTEGRATOR_H	

#include "common.h"
#include "model.h"
#include "history.h"
#include "coupling.h"
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
		population_model *model; //could be private
		population_coupling *coupling; //could be private
		global_connectivity_type connectivity; //could be private
		double dt;
		unsigned long n_nodes;
		virtual void scheme(unsigned int node, local_state_type &new_state)=0; // the actual integration scheme
		void step();
		void dfun_eval(	unsigned int node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &dphidt);

	public:
		global_history_type history; //TODO move to private when observers are implemented
		integrator(	population_model *model,
					population_coupling *coupling,
					const global_connectivity_type &connectivity,
					const global_history_type &initial_conditions,
					unsigned long n_nodes,
					double dt);
		void operator()(unsigned int n_steps);
};

class euler_deterministic : public integrator
{
	private:
		void scheme(unsigned int node, local_state_type &new_state);
	public:
		euler_deterministic(	population_model *model,
								population_coupling *coupling,
								const global_connectivity_type &connectivity,
								const global_history_type &initial_conditions,
								unsigned long n_nodes,
								double dt
							):integrator( 	model, coupling, connectivity, 
											initial_conditions, n_nodes, dt
							){};
};

#endif
