#ifndef MPI_HISTORY_H
#define MPI_HISTORY_H

#include <vector>
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <boost/math/special_functions/hermite.hpp>
#include "common.h"
#include "history.h"
#include "mpi.h"



class distributed_global_history: public global_history
{
	private:
		std::vector<double**> send_buffers;
		std::vector<double**> recv_buffers;
		neighbor_map_type recv_ids; 
		neighbor_map_type send_ids;
		std::size_t n_local_nodes;
		std::size_t n_vars;
		unsigned long step_id;
	public:
		distributed_global_history(
				std::vector< history_buffers* > history, 
				std::size_t n_vars,
				neighbor_map_type recv_ids,
				neighbor_map_type send_ids);
		virtual void push_state(global_state_type global_state);
		virtual std::size_t n_nodes(){return this->n_local_nodes;};
		static distributed_global_history* constant_initial_conditions( //TODO refactor to constructor, this is silly
				const global_connectivity_type &connectivity,
				const local_state_type &values,
				history_factory *history,
				population_model *model,
				const neighbor_map_type &recv_node_ids, 
				const neighbor_map_type &send_node_ids,
				double dt);

	
};

class distributed_scatter_gather_history: public distributed_global_history
{
	private:
		std::vector< std::vector< std::size_t > > region_nodes;
		std::vector< std::size_t > nodes_region;
	public:
		distributed_scatter_gather_history(
				std::vector< history_buffers* > history, 
				std::size_t n_vars,
				neighbor_map_type recv_ids,
				neighbor_map_type send_ids, 
				std::vector< std::vector< std::size_t > > region_nodes,
				std::vector< std::size_t > nodes_region):
			distributed_global_history(history,n_vars,recv_ids,send_ids),
			region_nodes(region_nodes),
			nodes_region(nodes_region){};
		void push_state(global_state_type global_state); //
		history_buffers *get_buffers(std::size_t node);
		std::size_t local_node_id(std::size_t global_node_id);
		std::size_t n_nodes(){return this->nodes_region.size();};
		static distributed_scatter_gather_history* constant_initial_conditions(
				const global_connectivity_type &connectivity,
				const std::vector< std::vector< std::size_t > > &region_nodes,
				const std::vector< std::size_t >&nodes_region,
				const local_state_type &values,
				history_factory *history,
				population_model *model,
				const neighbor_map_type &recv_node_ids, 
				const neighbor_map_type &send_node_ids,
				double dt
				);
		
};
#endif
