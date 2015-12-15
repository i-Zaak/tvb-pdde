#include "mpi_integrator.h"
#include "math.h"

mpi_integrator::mpi_integrator(	population_model *model,
								population_coupling *coupling,
								const global_connectivity_type &connectivity,
								const global_history_type &initial_conditions,
								solution_observer *observer,
								double dt,
								const neighbor_map_type &recv_node_ids, 
								const neighbor_map_type &send_node_ids):
				integrator( model, coupling, connectivity, initial_conditions, 
							observer, dt)
{
	// these hold the ids and oredring of overlapping nodes in the buffers
	this->recv_ids = recv_node_ids;
	this->send_ids = send_node_ids;

	// #local nodes = #all nodes - #shadow nodes
	this->n_nodes = this->history.size();
	for (unsigned long i = 0; i < this->recv_ids.size(); i++) {
		this->n_nodes = this->n_nodes - this->recv_ids[i].second.size();
	}

	// allocate continuous message buffers for communication with our neighbors
	this->send_buffers = std::vector<double**>(this->send_ids.size());
	for(std::vector<double**>::size_type i=0; i< send_buffers.size();i++){
		this->send_buffers[i] = alloc_2d_array(	this->send_ids[i].second.size(),
											this->model->n_vars());
	}
	this->recv_buffers = std::vector<double**>(this->recv_ids.size());
	for(std::vector<double**>::size_type i=0; i< recv_buffers.size();i++){
		this->recv_buffers[i] = alloc_2d_array(	this->recv_ids[i].second.size(),
											this->model->n_vars());
	}
}

mpi_integrator::~mpi_integrator()
{
	for(std::vector<double**>::size_type i=0; i< send_buffers.size();i++){
		free(this->send_buffers[i]);
	}

	for(std::vector<double**>::size_type i=0; i< recv_buffers.size();i++){
		free(this->recv_buffers[i]);
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

		// receive from neighbors
		std::vector<MPI_Request> reqs;
		reqs.reserve( send_ids.size() + recv_ids.size());
		for (unsigned long n_id=0; n_id < recv_ids.size(); n_id++) {
			int n_recv_nodes = recv_ids[n_id].second.size(); 
			int buf_length = n_recv_nodes * this->model->n_vars(); //this could be precomputed...
			MPI_Request req;
			MPI_Irecv( this->recv_buffers[n_id][0], buf_length, MPI_DOUBLE, this->recv_ids[n_id].first, i, MPI_COMM_WORLD, &req);
			reqs.push_back(req);
		}

		// send to neighbors
		for (unsigned long n_id=0; n_id < this->send_ids.size(); n_id++) {
			unsigned long n_send_nodes = this->send_ids[n_id].second.size();
			for(unsigned long j=0; j < n_send_nodes; j++){
				for (unsigned int dim = 0; dim < this->model->n_vars(); dim++) {
					this->send_buffers[n_id][j][dim] = global_state[ send_ids[n_id].first ][dim];
				}
			}
			int buf_length = n_send_nodes * this->model->n_vars();
			MPI_Request req;
			MPI_Isend(	this->send_buffers[n_id][0], buf_length,MPI_DOUBLE, this->send_ids[n_id].first, i, MPI_COMM_WORLD, &req );
			reqs.push_back(req);
		}

		//wait for sends and receives
		std::vector<MPI_Status>stats(reqs.size());
		MPI_Waitall(reqs.size(), &reqs[0], &stats[0]);

		// copy from receive buffer
		for (unsigned long n_id=0; n_id < this->recv_ids.size(); n_id++) {
			unsigned long n_recv_nodes = this->recv_ids[n_id].second.size();
			for(unsigned long j=0; j < n_recv_nodes; j++){
				for (unsigned int dim = 0; dim < this->model->n_vars(); dim++) {
					global_state[ recv_ids[n_id].first ][dim] = this->recv_buffers[n_id][j][dim];
				}
			}

		}
		

		// update history
		for(unsigned long node=0; node< this->n_nodes; node++){
			// this can be run only after all nodes are done
			this->history[node]->add_state(global_state[node]);
		}
		
	}

	// TODO gather results back to master (function of observer)
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



// More sophisticated (i.e. requiring communication) initial conditions to 
// come: e.g. for random IC broadcast only the seed, not the states.
