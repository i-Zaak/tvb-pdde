#include "connectivity.h"
#include "coupling.h" //for connection struct
#include <map>
#include <sstream>
#include "assert.h"

global_connectivity_type connectivity_from_mtx(std::ifstream &infile)
{
	while (infile.peek() == '%') infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	unsigned long n_nodes, m_nodes, nnz; 
	infile >> n_nodes >> m_nodes >> nnz; // matrix dimensions
	assert(n_nodes == m_nodes);
	
	global_connectivity_type connectivity = global_connectivity_type(n_nodes);

	for(unsigned long i=0; i < nnz; i++){
		unsigned long m, n;
		double delay;
		infile >> m >> n >> delay;
		
		// compensate mtx indexes
		n = n-1;
		m = m-1;


		connection conn = {
			n,		// from
			delay,	//delay
			1.0		// weigth
		};
		
		connectivity[m].push_back(conn);
	}

	return connectivity;
}

std::ifstream& seek_line(std::ifstream& file, unsigned long line_num){
    file.seekg(std::ios::beg);
    for(unsigned long i=0; i < line_num - 1; ++i){
	        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	    }
    return file;
}

unsigned long connectivity_from_partition(	std::ifstream &part_file,
											std::ifstream &conn_file,
											global_connectivity_type &connectivity,
											neighbor_map_type &recv_node_ids, 
											neighbor_map_type &send_node_ids )
{
	unsigned long part_id, n_part_nodes, n_buffered_nodes;
	part_file >> part_id >> n_part_nodes;
	connectivity.resize(n_part_nodes);

	// global ids -> local ids
	std::map<unsigned long, unsigned long> node_map=std::map<unsigned long,unsigned long>();

	// local nodes
	unsigned long node_id = 0;
	for (unsigned long i = 0; i < n_part_nodes; i++) {
		unsigned long node;
		part_file >> node;
		node_map.insert(std::make_pair(node,node_id));
		node_id++;
	}

	// remote nodes
	n_buffered_nodes = n_part_nodes;
	unsigned long neigh_id, conn_type, n_neigh_nodes;
	//while(!part_file.eof()){
	while( part_file >> neigh_id >> conn_type >> n_neigh_nodes){
		if(conn_type == 0){
			//receive
			std::vector<unsigned long> recv_ids(n_neigh_nodes);
			//connectivity.resize(connectivity.size() + n_neigh_nodes);
			for (unsigned long i = 0; i < n_neigh_nodes; i++) {
				unsigned long node; 
				part_file >> node;
				node_map.insert(std::make_pair(node,node_id));
				recv_ids[i] = node_id;
				node_id++;
			}
			recv_node_ids.push_back(std::make_pair(neigh_id,recv_ids));
			n_buffered_nodes += recv_ids.size();
		}else{
			//send
			std::vector<unsigned long> send_ids(n_neigh_nodes);
			for (unsigned long i = 0; i < n_neigh_nodes; i++) {
				unsigned long node; 
				part_file >> node;
				send_ids[i] = node_map[node];
			}
			send_node_ids.push_back(std::make_pair(neigh_id,send_ids));
		}
	}

	// connect them 
	unsigned long n_nodes, n_conns, type;
	conn_file >> n_nodes >> n_conns >> type;
	
	unsigned long n_line = 0;
	std::string line;
	std::getline(conn_file, line); //just mill the endline really...
	while(std::getline(conn_file, line)) {
		std::map<unsigned long,unsigned long>::iterator node_it = node_map.find(n_line);
		n_line++;
		if( node_it != node_map.end() && node_it->second < n_part_nodes){
			local_connectivity_type lconn = local_connectivity_type();
			std::istringstream line_str(line);   
			while (!line_str.eof()) {
				unsigned long from;
				double delay;
				line_str >> from >> delay;
				from = node_map[from-1]; // map to local id
				connection conn = {
					from,		// from
					delay,	//delay
					1.0		// weigth
				};
				lconn.push_back(conn);

			}
			connectivity[node_it->second] = lconn;
		}
	}
	connectivity.resize(n_buffered_nodes);
	return n_part_nodes;
}



std::size_t read_regional_mapping(	std::ifstream& region_file,
									//std::ifstream& conn_file,
									//global_connectivity_type &connectivity,
									std::vector< std::vector< std::size_t > > &region_nodes,
									std::vector< std::size_t >&nodes_region)
{
	std::size_t n_nodes, n_regions;
	region_file >> n_nodes >> n_regions;
	region_nodes.resize(n_regions);
	nodes_region.resize(n_nodes);
	//connectivity.resize(n_regions);

	// read the mapping
	for (std::size_t i = 0; i < n_nodes; i++) {
		std::size_t reg;
		region_file >> reg;
		nodes_region[i] = reg;
		region_nodes[reg].push_back(i);
	}

	// read the connectivity
	//global_connectivity_type conn = connectivity_from_mtx(conn_file);
	//connectivity = conn;

	return n_regions; //what for?
}

void trivial_regional_mapping(
		std::size_t n_nodes,
		std::vector< std::vector< std::size_t > > &region_nodes,
		std::vector< std::size_t >&nodes_region)
{
	region_nodes.resize(1);
	nodes_region.resize(n_nodes);

	for (std::size_t i = 0; i < n_nodes; i++) {
		nodes_region[i] = 0;
		region_nodes[0].push_back(i);
	}
}
