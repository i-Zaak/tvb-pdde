#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>

#include "common.h"
#include "connectivity.h"
#include "coupling.h"
#include "history.h"
#include "mpi_integrator.h"
#include "model.h"
#include "observer.h"
#include "mpi.h"

double to_seconds(timeval start, timeval stop){
	return stop.tv_sec - start.tv_sec + 1e-6 * (stop.tv_usec - start.tv_usec);
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc,&argv); // clean the argv from MPI related stuff

	int task_id;
	MPI_Comm_rank(MPI_COMM_WORLD,&task_id);

	if (argc != 5) {
		if(task_id == 0){
			std::cout << "usage: " << argv[0] << " connectivity_file.adj partition_file.adj.n dt n_steps" << std::endl;
		}
		return 1;
	}
	std::ifstream conn_file(argv[1]);
	if(!conn_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << argv[1] << std::endl;
		}
		return 2;
	}
	std::ostringstream stream; stream << task_id; // refactor when switched to C++0x
	std::string part_filename = std::string(argv[2]) + "." + stream.str();
	std::ifstream part_file(part_filename.c_str());
	if(!part_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << part_filename << std::endl;
		}
		return 3;
	}


	double dt = atof(argv[3]);
	int n_steps = atoi(argv[4]);

	
	neighbor_map_type recv_node_ids; 
	neighbor_map_type send_node_ids;
	global_connectivity_type connectivity;

	struct timeval start_t, io_end_t, init_end_t, comp_end_t;

	gettimeofday(&start_t, NULL);
	// load the partitioning
	unsigned long n_local_nodes = connectivity_from_partition( 	part_file, 
																conn_file, 
																connectivity, 
																recv_node_ids, 
																send_node_ids );
	gettimeofday(&io_end_t, NULL);

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
	gettimeofday(&init_end_t, NULL);

	integrator(n_steps);
	//TODO save results
	gettimeofday(&comp_end_t, NULL);
	
	int status = MPI_Finalize();
	if(task_id == 0){
		std::cout << "connectivity loading: " << to_seconds( start_t,io_end_t) << "s" << std::endl; 
		std::cout << "initialization: " << to_seconds( io_end_t,init_end_t) << "s" << std::endl; 
		std::cout << "computation: " << to_seconds(init_end_t,comp_end_t) << "s" << std::endl; 
	}

	return status;
}
