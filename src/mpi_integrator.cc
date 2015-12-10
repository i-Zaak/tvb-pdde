#include "mpi_integrator.h"
#include "math.h"

mpi_integrator::mpi_integrator(	population_model *model,
								population_coupling *coupling,
								global_connectivity_type worker_connectivity,
								global_history_type initial_conditions,
								node_map_type node_map,
								neighbor_map_type in_ids,
								neighbor_map_type out_ids,
								solution_observer *observer,
								double dt)
{
	this->model = model;
	this->coupling = coupling;
	this->history = initial_conditions;
	// worker relative numbering
	this->worker_connectivity = worker_connectivity;
	// these hold the ids and oredring of overlapping nodes in the buffers
	this->neigbor_in_ids = in_ids;
	this->neigbor_out_ids = out_ids;

	this->observer = observer;
	this->dt = dt;
	this->n_nodes = initial_conditions.size();

	// WORKERS' stuff
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	// allocate continuous message buffers for communication with our neighbors
	this->send_buffers = std::vector<double**>(this->neighbor_out_ids.size());
	for(std::vector<double**>::size_type i=0; i< send_buffers.size();i++){
		this->send_buffers[i] = alloc_2d_array(	this->neighbor_out_ids[i].size(),
											this->model->n_vars());
	}
	this->recv_buffers = std::vector<double**>(this->neighbor_in_ids.size());
	for(std::vector<double**>::size_type i=0; i< recv_buffers.size();i++){
		this->recv_buffers[i] = alloc_2d_array(	this->neighbor_in_ids[i].size(),
											this->model->n_vars());
	}
}

void mpi_integrator::operator()(unsigned int n_steps)
{
	
	
	for(unsigned int i=0;i<n_steps;i++) {
		global_state_type global_state = global_state_type(this->n_nodes);
		local_state_type local_state = local_state_type(this->model->n_vars());
		
		// compute new state phi(t_{n+1})
		for(unsigned long node=0; node< this->n_nodes; node++){
			double time; //of the next step
			time = scheme(node, local_state); 
			global_state[node] = local_state;

			// observe and behold
			(*this->observer)(node, local_state, time);
		}

		// for every neighbor:
		// build the message matrix
		for (std::vector<int>::size_type n = 0; n < neighbor_ids.size(); n++) {
			for (local_state::size_type v = 0; v < this->model->n_vars(); v++) {
				local_state_buffer[n][v] = global_state[node][v];
			}
		}
		// send to neighbor

		// update history
		for(unsigned long node=0; node< this->n_nodes; node++){
			// this can be run only after all nodes are done
			this->history[node]->add_state(global_state[node]);
		}
	}

	// gather results back to master (function of observer)
}

	

/**
 Wraps the coupling and local model evaluation to one reasonable function call.
 To be used in the integrator scheme instead of direct calls to model/coupling.
 */
void mpi_integrator::dfun_eval(	unsigned int node,
							const local_state_type phi, 
							double time_offset, 
							local_state_type &dphidt)
{
	local_coupling_type l_coupling = local_coupling_type(this->model->n_vars());
	(*this->coupling)( this->connectivity[node], 
					this->history, 
					l_coupling,
					time_offset);

	(*this->model)(	phi,
					l_coupling,
					dphidt);
}

double mpi_euler_deterministic::scheme(unsigned int node, local_state_type &new_state)
{
	local_state_type phi = this->history[node]->get_values_at(0); // current 
	local_state_type dphidt = local_state_type(this->model->n_vars());
	this->dfun_eval(node, phi, 0.0, dphidt);
	for(local_state_type::size_type dim=0; dim<phi.size();dim++){
		new_state[dim] = phi[dim] + this->dt * dphidt[dim];
	}

	return this->dt; //eqidistant timestepping here
}


global_history_type mpi_integrator::constant_initial_conditions(
		const global_connectivity_type &connectivity,
		const local_state_type &values,
		history_factory* history,
		population_model* model,
		double dt)
{

	global_history_type initial_conditions = global_history_type(connectivity.size());
	
	// determine buffer lengths
	std::vector<double> max_delays = std::vector<double>(connectivity.size(),0.0);
	for(global_history_type::size_type i=0; i < initial_conditions.size(); i++){
		for(local_connectivity_type::size_type j=0; j< connectivity[i].size(); j++)	{
			connection conn = connectivity[i][j];
			if ( max_delays[conn.from] < conn.delay) {
				max_delays[conn.from] = conn.delay;
			}
		}
	}

	// create buffers and fill with constant state
	for(global_history_type::size_type i=0; i < initial_conditions.size(); i++){
		int length = ceil(max_delays[i] / dt)+1;
		initial_conditions[i] = history->create_history(length, dt, model->n_vars());
		for(int j = 0; j < length; j++) {
			initial_conditions[i]->add_state(values);
		}
	}

	return initial_conditions;
}

