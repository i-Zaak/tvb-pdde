#ifndef INTEGRATOR_H
#define INTEGRATOR_H	

#include "common.h"
#include "model.h"
#include "history.h"
#include "coupling.h"
#include "observer.h"
#include "random.h"
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
		std::vector<global_connectivity_type> connectivities; //could be private
		double dt;
		unsigned long n_nodes;
		// the actual integration scheme, performs one step and returns the
		// length of the step in time
		virtual double scheme(unsigned long node, local_state_type &new_state)=0; 
		void step();
		void dfun_eval(	unsigned long node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &df,
						local_state_type &dg);

	public:
		global_history *scheme_history;
		std::vector<global_history*> histories; //TODO move to private when observers are implemented
		integrator(	population_model *model,
					population_coupling *coupling,
					const global_connectivities_type &connectivities,
					const global_histories_type &initial_conditions,
					solution_observer *observer,
					double dt);

		void operator()(unsigned long n_steps);
};

class euler : public integrator
{
	private:
		double scheme(unsigned long node, local_state_type &new_state);
	public:
		euler(	population_model *model,
				population_coupling *coupling,
				const std::vector<global_connectivity_type> &connectivities,
				const std::vector<global_history*> &initial_conditions,
				solution_observer *observer,
				double dt
			 ):integrator( 	model, coupling, connectivities, 
				 initial_conditions, observer,
				 dt
				 ){};
};
class euler_maruyama : public integrator
{
	private:
		double scheme(unsigned long node, local_state_type &new_state);
		rng *noise_generator;
	public:
		euler_maruyama(		population_model *model,
							population_coupling *coupling,
							const std::vector<global_connectivity_type> &connectivities,
							const std::vector<global_history*> &initial_conditions,
							solution_observer *observer,
							rng *noise_generator,
							double dt
						);
};

#endif
