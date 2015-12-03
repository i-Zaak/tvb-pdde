#include "connectivity.h"
#include "coupling.h" //for connection struct
#include <fstream>
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
