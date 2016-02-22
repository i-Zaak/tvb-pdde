#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>

#include "common.h"
#include "connectivity.h"
#include "coupling.h"
#include "integrator.h"
#include "history.h"
#include "mpi_history.h"
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
			std::cout << "usage: " << argv[0] ;
			std::cout << " surface_connectivity_file.adj"; // 1 
			std::cout << " surface_partition_file.adj.n"; // 2
			std::cout << " region_connectivity_file.adj"; // 3 
			std::cout << " region_partition_file.adj.n"; // 4
			std::cout << " dt"; // 5
			std::cout << " n_steps" << std::endl; // 6
		}
		return 1;
	}
	std::ifstream surf_conn_file(argv[1]);
	if(!surf_conn_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << argv[1] << std::endl;
		}
		return 2;
	}
	std::ostringstream stream; stream << task_id; // refactor when switched to C++0x
	std::string surf_part_filename = std::string(argv[2]) + "." + stream.str();
	std::ifstream surf_part_file(surf_part_filename.c_str());
	if(!surf_part_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << surf_part_filename << std::endl;
		}
		return 3;
	}
	std::ifstream reg_conn_file(argv[3]);
	if(!reg_conn_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << argv[3] << std::endl;
		}
		return 4;
	}
	stream.str(""); 
	stream << task_id; // refactor when switched to C++0x
	std::string reg_part_filename = std::string(argv[4]) + "." + stream.str();
	std::ifstream reg_part_file(reg_part_filename.c_str());
	if(!reg_part_file.is_open()) {
		if(task_id == 0){
			std::cout << "could not open " << reg_part_filename << std::endl;
		}
		return 5;
	}


	double dt = atof(argv[5]);
	unsigned long n_steps = atoi(argv[6]);

	
	neighbor_map_type s_recv_node_ids, r_recv_node_ids; 
	neighbor_map_type s_send_node_ids, r_send_node_ids;
	global_connectivity_type surf_connectivity, reg_connectivity;
	std::vector< std::vector< std::size_t > > region_nodes;
	std::vector< std::size_t >nodes_region;

	struct timeval start_t, io_end_t, init_end_t, comp_end_t;

	gettimeofday(&start_t, NULL);
	// load the partitioning
	unsigned long n_local_nodes = connectivity_from_partition( 	surf_part_file, 
																surf_conn_file, 
																surf_connectivity, 
																s_recv_node_ids, 
																s_send_node_ids );
	unsigned long n_reg_nodes = connectivity_from_partition( 	reg_part_file, 
																reg_conn_file, 
																reg_connectivity, 
																r_recv_node_ids, 
																r_send_node_ids );
	trivial_regional_mapping( n_local_nodes, region_nodes, nodes_region );
	global_connectivities_type connectivities = global_connectivities_type(); 
	connectivities[0] = surf_connectivity;
	connectivities[1] = reg_connectivity;

	gettimeofday(&io_end_t, NULL);

	// initialize history for this process, also for the shadow elements
	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),1.5);
	// this may do some communication inside, here we don't care...
	//global_history_type initial_conditions = mpi_integrator::constant_initial_conditions(
	//		connectivity, n_local_nodes,
	//		values, history, model, dt ); 
	global_histories_type initial_conditions = global_histories_type(2);
	initial_conditions[0] = distributed_global_history::constant_initial_conditions(
			surf_connectivity, values, history, model, s_recv_node_ids, s_send_node_ids, dt );
	initial_conditions[1] = distributed_scatter_gather_history::constant_initial_conditions(
			reg_connectivity, region_nodes, nodes_region, values, history, model, r_recv_node_ids, r_send_node_ids, dt );

	
	// create the integrator for this process
	linear_coupling *coupling = new linear_coupling();
	raw_observer *observer = new raw_observer(n_local_nodes);
	euler integrator = euler(
			model, coupling, connectivities, initial_conditions, observer,
			dt);

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
