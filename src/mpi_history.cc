#include "mpi_history.h"

distributed_global_history::distributed_global_history(
		std::vector< history_buffers* > history,
		std::size_t n_vars,
		neighbor_map_type recv_ids,
		neighbor_map_type send_ids):
			global_history(history),
			recv_ids(recv_ids),
			send_ids(send_ids)
{
	this->step_id = 0;
	this->n_vars = n_vars;
	// #local nodes = #all nodes - #shadow nodes
	this->n_local_nodes = this->history.size();
	for (unsigned long i = 0; i < this->recv_ids.size(); i++) {
		this->n_local_nodes = this->n_local_nodes - this->recv_ids[i].second.size();
	}

	// allocate continuous message buffers for communication with our neighbors
	this->send_buffers = std::vector<double**>(this->send_ids.size());
	for(std::vector<double**>::size_type i=0; i< send_buffers.size();i++){
		this->send_buffers[i] = alloc_2d_array(	this->send_ids[i].second.size(),
											this->n_vars);
	}
	this->recv_buffers = std::vector<double**>(this->recv_ids.size());
	for(std::vector<double**>::size_type i=0; i< recv_buffers.size();i++){
		this->recv_buffers[i] = alloc_2d_array(	this->recv_ids[i].second.size(),
											this->n_vars);
	}
}
distributed_global_history* distributed_global_history::constant_initial_conditions(
    const global_connectivity_type &connectivity,
    const local_state_type &values,
    history_factory *history,
    population_model *model,
    const neighbor_map_type &recv_node_ids, 
    const neighbor_map_type &send_node_ids,
    double dt)
{
  std::vector< history_buffers* > buffers = buffers_from_connectivity(connectivity,values,history,model,dt);
	distributed_global_history *ghist = new distributed_global_history(buffers, model->n_vars(),recv_node_ids, send_node_ids);
	return ghist;

}


void distributed_global_history::push_state(global_state_type global_state)
{
	
	// receive from neighbors
	std::vector<MPI_Request> reqs;
	reqs.reserve( send_ids.size() + recv_ids.size());
	for (unsigned long n_id=0; n_id < recv_ids.size(); n_id++) {
		unsigned long n_recv_nodes = recv_ids[n_id].second.size(); 
		unsigned long buf_length = n_recv_nodes * this->n_vars; //this could be precomputed...
		MPI_Request req;
		MPI_Irecv( this->recv_buffers[n_id][0], buf_length, MPI_DOUBLE, this->recv_ids[n_id].first, this->step_id, MPI_COMM_WORLD, &req);
		reqs.push_back(req);
	}

	// send to neighbors
	for (unsigned long n_id=0; n_id < this->send_ids.size(); n_id++) {
		unsigned long n_send_nodes = this->send_ids[n_id].second.size();
		for(unsigned long j=0; j < n_send_nodes; j++){
			for (unsigned int dim = 0; dim < this->n_vars; dim++) {
				this->send_buffers[n_id][j][dim] = global_state[ send_ids[n_id].first ][dim];
			}
		}
		unsigned long buf_length = n_send_nodes * this->n_vars;
		MPI_Request req;
		MPI_Isend(	this->send_buffers[n_id][0], buf_length,MPI_DOUBLE, this->send_ids[n_id].first, this->step_id, MPI_COMM_WORLD, &req );
		reqs.push_back(req);
	}

	local_state_type local_state = local_state_type(this->n_vars);
	//wait for sends and receives
	std::vector<MPI_Status>stats(reqs.size());
	MPI_Waitall(reqs.size(), &reqs[0], &stats[0]);
	// copy from receive buffer
	for (unsigned long n_id=0; n_id < this->recv_ids.size(); n_id++) {
		unsigned long n_recv_nodes = this->recv_ids[n_id].second.size();
		for(unsigned long j=0; j < n_recv_nodes; j++){
			for (unsigned int dim = 0; dim < this->n_vars; dim++) {
				local_state[dim] = this->recv_buffers[n_id][j][dim];
				
			}
			this->history[ recv_ids[n_id].second[j] ]->add_state(local_state);
		}

	}
	

	// update history
	for(unsigned long node=0; node< this->n_local_nodes; node++){
		// this can be run only after all nodes are done
		this->history[node]->add_state(global_state[node]);
	}
		

}
history_buffers *distributed_scatter_gather_history::get_buffers(std::size_t node)
{
	return this->history[this->nodes_region[node]];
}

void distributed_scatter_gather_history::push_state(global_state_type global_state)
{
	// compute local sums and call parent push_state
	global_state_type region_state = global_state_type(this->region_nodes.size());
	for(std::size_t i=0; i < this->region_nodes.size(); i++){
		std::size_t regsize = this->region_nodes[i].size();
		if(regsize == 0) continue;

		//TODO omp parallel sum
		unsigned int ndim = region_state[0].size();
		local_state_type mean=local_state_type(ndim,0.0);
		for(std::size_t j=0; j < regsize; j++){
			for (unsigned int dim = 0; dim < ndim; dim++) {
				mean[dim] += region_state[ this->region_nodes[i][j]][dim] / double(regsize);
			}
		}
		region_state[i] = mean;
	}
	distributed_scatter_gather_history::push_state(region_state);
	//
	// TODO: let go only if we have received the states for next step (not necessarily from this step)
	
}

std::size_t distributed_scatter_gather_history::local_node_id(std::size_t global_node_id)
{
	return this->nodes_region[global_node_id];
}

distributed_scatter_gather_history* distributed_scatter_gather_history::constant_initial_conditions(
		const global_connectivity_type &connectivity,
		const std::vector< std::vector< std::size_t > > &region_nodes,
		const std::vector< std::size_t >&nodes_region,
		const local_state_type &values,
		history_factory *history,
		population_model *model,
		const neighbor_map_type &recv_node_ids, 
		const neighbor_map_type &send_node_ids,
		double dt
		)
{

  std::vector< history_buffers* > buffers = buffers_from_connectivity(connectivity,values,history,model,dt);
	distributed_scatter_gather_history *ghist = new distributed_scatter_gather_history(buffers, model->n_vars(), recv_node_ids, send_node_ids, region_nodes, nodes_region);
	return ghist;

}
