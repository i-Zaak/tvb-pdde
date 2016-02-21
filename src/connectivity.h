#ifndef H_CONNECTIVITY
#define H_CONNECTIVITY

#include "common.h"
#include <string>
#include <fstream>

struct connection
{
	unsigned long from;
	double delay;
	double weight;
};
global_connectivity_type connectivity_from_mtx(std::ifstream& part_file);

unsigned long connectivity_from_partition(	std::ifstream& part_file, 
									std::ifstream& conn_file,
									global_connectivity_type &connectivity,
									neighbor_map_type &recv_node_ids, 
									neighbor_map_type &send_node_ids );

std::size_t read_regional_mapping(	std::ifstream& region_file,
									//std::ifstream& conn_file,
									//global_connectivity_type &connectivity,
									std::vector< std::vector< std::size_t > > &region_nodes,
									std::vector< std::size_t >&nodes_region);
#endif
