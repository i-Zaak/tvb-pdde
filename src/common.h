#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <stdlib.h>
class history_buffers;

// history buffers for state variables for all populations
typedef std::vector< history_buffers* > global_history_type; 

// state variables of single population
typedef std::vector<double> local_state_type; 
// state variables of all population
typedef std::vector< std::vector<double> > global_state_type; 

// coupling variables of all populations
typedef std::vector< std::vector<double> > global_coupling_type;
// coupling variables of single population
typedef std::vector<double>  local_coupling_type;

struct connection;
// connectivity pattern between populations: to x from 
typedef std::vector< std::vector< connection > > global_connectivity_type; 
// connectivity pattern for single population 
typedef std::vector< connection > local_connectivity_type; 

// observed global solution nodes x times x (time,local_state)
typedef std::vector< std::vector< std::pair< double, std::vector<double> > > > global_solution_type;
// observed local solution times x (time,local_state)
typedef std::vector< std::pair< double, std::vector<double> > > local_solution_type;
// observed solution at single time step (time,local_state)
typedef std::pair< double, std::vector<double> > solution_pair_type;

//maps nodes to neighboring mpi thread ids
typedef std::vector< std::pair<unsigned long,std::vector<unsigned long> > > neighbor_map_type;

// mapping of node ids
typedef std::vector<unsigned long> node_map_type;

// allocator for continuous 2D array needed for MPI code
inline double **alloc_2d_array(unsigned long n, unsigned long m) {
    double *data = (double *)malloc(n*m*sizeof(double));
    double **array = (double **)malloc(n*sizeof(double*));
    for(unsigned long i=0; i<n; i++){
		array[i] = &(data[m*i]);
	}
    return array;
};

// allocator for 1D array
inline double *alloc_1d_array(unsigned long n) {
    double *array = (double *)malloc(n*sizeof(double));
    return array;
};

#endif
