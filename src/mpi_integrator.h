#ifndef INTEGRATOR_H
#define INTEGRATOR_H	

#include "common.h"
#include "model.h"
#include "history.h"
#include "coupling.h"
#include "observer.h"
#include "mpi.h"

/**
 * Same as the `integrator` class, but distributed using MPI. To be possibly
 * refactored in future reusing the common parts. 
 *
 * Assumes, that MPI_Init was called outside.
 */
class mpi_integrator 
{
	protected:
		population_model *model; //could be private
		population_coupling *coupling; //could be private
		solution_observer *observer; //could be private
		global_connectivity_type connectivity; //could be private
		global_history_type history; 
		double dt;
		unsigned long n_nodes;
		// the actual integration scheme, performs one step and returns the
		// length of the step in time
		virtual double scheme(unsigned int node, local_state_type &new_state)=0; 
		void step();
		void dfun_eval(	unsigned int node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &dphidt);
		void distribute_initial_state;

	public:
		mpi_integrator(	population_model *model,
					population_coupling *coupling,
					const global_connectivity_type &connectivity,
					const global_history_type &initial_conditions,
					solution_observer *observer,
					double dt);
		static global_history_type constant_initial_conditions(
				const global_connectivity_type &connectivity,
				const local_state_type &values,
				history_factory* history,
				population_model* model,
				double dt);

		void operator()(unsigned int n_steps);
};

class mpi_euler_deterministic : public mpi_integrator
{
	private:
		double scheme(unsigned int node, local_state_type &new_state);
	public:
		mpi_euler_deterministic(	population_model *model,
								population_coupling *coupling,
								const global_connectivity_type &connectivity,
								const global_history_type &initial_conditions,
								solution_observer *observer,
								double dt
							):mpi_integrator( 	model, coupling, connectivity, 
											initial_conditions, observer,
											dt
							){};
};
#endif
