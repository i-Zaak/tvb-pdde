#ifndef MPI_INTEGRATOR_H
#define MPI_INTEGRATOR_H	

#include "common.h"
#include "model.h"
#include "history.h"
#include "coupling.h"
#include "observer.h"
#include "mpi.h"
#include "integrator.h"
#include <vector>

/**
 * Same as the `integrator` class, but distributed using MPI. To be possibly
 * refactored in future reusing the common parts. 
 *
 * Assumes, that MPI_Init was called outside.
 */
class mpi_integrator: public integrator
{
	protected:
		std::vector<double**> send_buffers;
		std::vector<double**> recv_buffers;
		neighbor_map_type recv_ids; 
		neighbor_map_type send_ids;
		// the actual integration scheme, performs one step and returns the
		// length of the step in time
		virtual double scheme(unsigned long node, local_state_type &new_state)=0; 
		void step();
		void dfun_eval(	unsigned long node,
						const local_state_type phi, 
						double time_offset, 
						local_state_type &dphidt);
	public:
		mpi_integrator(	population_model *model,
					population_coupling *coupling,
					const global_connectivity_type &connectivity,
					const global_history_type &initial_conditions,
					solution_observer *observer,
					double dt,
					const neighbor_map_type &recv_node_ids, 
					const neighbor_map_type &send_node_ids);

		~mpi_integrator();
		void operator()(unsigned long n_steps);
};

// this duplicates euler_deterministic. Refactor with multiple inheritance?
class mpi_euler_deterministic : public mpi_integrator
{
	private:
		double scheme(unsigned long node, local_state_type &new_state);
	public:
		mpi_euler_deterministic(	population_model *model,
								population_coupling *coupling,
								const global_connectivity_type &connectivity,
								const global_history_type &initial_conditions,
								solution_observer *observer,
								double dt,
					const neighbor_map_type &recv_node_ids, 
					const neighbor_map_type &send_node_ids
							):mpi_integrator( 	model, coupling, connectivity, 
											initial_conditions, observer,
											dt,
					recv_node_ids, 
					send_node_ids
							){};
};
#endif
