#include "connectivity.h"
#include "coupling.h" //for connection struct
#include <map>
#include <sstream>
#include "assert.h"

global_connectivity_type connectivity_from_mtx(std::string filename)
{
	std::ifstream infile(filename.c_str());
	while (infile.peek() == '%') infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	unsigned long n_nodes, m_nodes, nnz; 
	infile >> n_nodes >> m_nodes >> nnz; // matrix dimensions
	assert(n_nodes == m_nodes);
	
	global_connectivity_type connectivity = global_connectivity_type(n_nodes);

	for(unsigned long i=0; i < nnz; i++){
		int m, n;
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

std::ifstream& seek_line(std::ifstream& file, unsigned int line_num){
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
	unsigned long part_id, n_part_nodes;
	part_file >> part_id >> n_part_nodes;
	connectivity.resize(n_part_nodes);

	// global ids -> local ids
	std::map<int, int> node_map=std::map<int,int>();

	// local nodes
	unsigned long node_id = 0;
	for (unsigned long i = 0; i < n_part_nodes; i++) {
		int node;
		part_file >> node;
		node_map.insert(std::make_pair(node,node_id));
		node_id++;
	}

	// remote nodes 
	while(!part_file.eof()){
		unsigned long neigh_id, conn_type, n_neigh_nodes;
		part_file >> neigh_id >> conn_type >> n_neigh_nodes;
		if(conn_type == 0){
			//receive
			std::vector<int> recv_ids(n_neigh_nodes);
			//connectivity.resize(connectivity.size() + n_neigh_nodes);
			for (unsigned long i = 0; i < n_neigh_nodes; i++) {
				int node; 
				part_file >> node;
				node_map.insert(std::make_pair(node,node_id));
				recv_ids[i] = node_id;
				node_id++;
			}
			recv_node_ids.push_back(std::make_pair(neigh_id,recv_ids));
		}else{
			//send
			std::vector<int> send_ids(n_neigh_nodes);
			for (unsigned long i = 0; i < n_neigh_nodes; i++) {
				int node; 
				part_file >> node;
				send_ids[i] = node_map[node];
			}
			send_node_ids.push_back(std::make_pair(neigh_id,send_ids));
		}
	}

	// connect them 
	unsigned long n_nodes, n_conns, type;
	conn_file >> n_nodes >> n_conns >> type;
	for(	std::map<int,int>::iterator node_it=node_map.begin();
			node_it != node_map.end();
			node_it++){
		if(node_it->second < n_part_nodes){ // we want to connect only local nodes
			seek_line(conn_file,node_it->first+2);
			std::string line;
			//read the incomming connections
			local_connectivity_type lconn = local_connectivity_type();
			std::getline(conn_file, line);
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
	return n_part_nodes;
}
