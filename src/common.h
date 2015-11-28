#include <vector>
class history_buffers;

// history buffers for state variables for all populations
typedef std::vector< history_buffers* > global_history_type; 

// state variables of single population
typedef std::vector<double> local_state_type; 

// coupling variables of all populations
typedef std::vector< std::vector<double> > global_coupling_type;

// coupling variables of single population
typedef std::vector<double>  local_coupling_type;

struct connection;
// connectivity pattern between populations: to x from 
typedef std::vector< std::vector< connection > > global_connectivity_type; 

// connectivity pattern for single population 
typedef std::vector< connection > local_connectivity_type; 
