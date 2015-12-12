#ifndef H_CONNECTIVITY
#define H_CONNECTIVITY

#include "common.h"
#include <string>
#include <fstream>

struct connection
{
	int from;
	//int to;
	double delay;
	double weight;
};
global_connectivity_type connectivity_from_mtx(std::string filename);
unsigned long connectivity_from_partition(	std::ifstream& part_file, 
									std::ifstream& conn_file,
									global_connectivity_type &connectivity,
									neighbor_map_type &recv_node_ids, 
									neighbor_map_type &send_node_ids );

#endif
