#ifndef INTEGRATOR_H
#define INTEGRATOR_H	

#include "common.h"
#include "model.h"
#include "history.h"
#include "coupling.h"
#include "observer.h"
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
		solution_observer *observer; //could be private
		global_connectivity_type connectivity; //could be private
		double dt;
		unsigned long n_nodes;
		// the actual integration scheme, performs one step and returns the
		// length of the step in time
		virtual double scheme(unsigned int node, local_state_type &new_state)=0; 
		void step();
		void dfun_eval(	unsigned int node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &df,
						local_state_type &dg);

	public:
		global_history_type history; //TODO move to private when observers are implemented
		integrator(	population_model *model,
					population_coupling *coupling,
					const global_connectivity_type &connectivity,
					const global_history_type &initial_conditions,
					solution_observer *observer,
					double dt);
		static global_history_type constant_initial_conditions(
				const global_connectivity_type &connectivity,
				unsigned long n_nodes, // at least connectivity.size()
				const local_state_type &values,
				history_factory* history,
				population_model* model,
				double dt);

		void operator()(unsigned int n_steps);
};

class stochastic_integrator : pulblic integrator
{

};

class euler : public integrator
{
	private:
		double scheme(unsigned int node, local_state_type &new_state);
	public:
		euler(	population_model *model,
								population_coupling *coupling,
								const global_connectivity_type &connectivity,
								const global_history_type &initial_conditions,
								solution_observer *observer,
								double dt
							):integrator( 	model, coupling, connectivity, 
											initial_conditions, observer,
											dt
							){};
};

#endif
