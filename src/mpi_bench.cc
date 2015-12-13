#include <iostream>
#include <sstream>
#include <string>

#include "common.h"
#include "connectivity.h"
#include "coupling.h"
#include "history.h"
#include "mpi_integrator.h"
#include "model.h"
#include "observer.h"
#include "mpi.h"


int main(int argc, char* argv[])
{
	MPI_Init(&argc,&argv); // clean the argv from MPI related stuff

	int task_id;
	MPI_Comm_rank(MPI_COMM_WORLD,&task_id);

	if (argc != 5) {
		std::cout << "usage: " << argv[0] << " connectivity_file.adj partition_file.adj.n dt n_steps" << std::endl;
		return 1;
	}
	std::ifstream conn_file(argv[1]);
	if(!conn_file.is_open()) {
		std::cout << "could not open " << argv[1] << std::endl;
		return 2;
	}
	std::ostringstream stream; stream << task_id; // refactor when switched to C++0x
	std::string part_filename = std::string(argv[2]) + "." + stream.str();
	std::ifstream part_file(part_filename.c_str());
	if(!part_file.is_open()) {
		std::cout << "could not open " << part_filename << std::endl;
		return 3;
	}


	double dt = atof(argv[3]);
	int n_steps = atoi(argv[4]);

	
	neighbor_map_type recv_node_ids; 
	neighbor_map_type send_node_ids;
	global_connectivity_type connectivity;

	// load the partitioning
	unsigned long n_local_nodes = connectivity_from_partition( 	part_file, 
																conn_file, 
																connectivity, 
																recv_node_ids, 
																send_node_ids );

	// initialize history for this process, also for the shadow elements
	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),1.5);
	// this may do some communication inside, here we don't care...
	global_history_type initial_conditions = mpi_integrator::constant_initial_conditions(
			connectivity, n_local_nodes,
			values, history, model, dt ); 

	
	// create the integrator for this process
	linear_coupling *coupling = new linear_coupling();
	raw_observer *observer = new raw_observer(connectivity.size());
	mpi_euler_deterministic integrator = mpi_euler_deterministic(
			model, coupling, connectivity, initial_conditions, observer,
			dt,recv_node_ids,send_node_ids);

	MPI_Barrier(MPI_COMM_WORLD);

	integrator(n_steps);
	//TODO save results
	
	MPI_Finalize();
	return 0;
}
