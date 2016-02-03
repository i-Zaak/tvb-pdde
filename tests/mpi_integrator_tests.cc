#include "catch.hpp"
#include "mpi_integrator.h"
#include <stdio.h>

TEST_CASE("Distributed initial conditions from partitioned connectivity" "[mpi integrator]")
{
	
	int task_id, size;
    MPI_Comm_rank (MPI_COMM_WORLD, &task_id);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    CHECK(size == 4); //required for partitioned test inputs
    CHECK(task_id < 4); //sanity check...

	std::ifstream conn_file("data/test_init.adj");
	CHECK(conn_file.is_open());

	//test_init.adj.part.4.[0-3]
	char part_filename[25];
	sprintf(part_filename, "data/test_init.adj.part.4.%d", task_id);

	std::ifstream part_file(part_filename);
	CHECK(part_file.is_open());

	double dt=0.2;

	neighbor_map_type recv_node_ids; 
	neighbor_map_type send_node_ids;
	global_connectivity_type connectivity;

	unsigned long n_local_nodes = connectivity_from_partition( 	part_file, 
															conn_file, 
															connectivity, 
															recv_node_ids, 
															send_node_ids );
	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),1.5);
	global_history_type initial_conditions = mpi_integrator::constant_initial_conditions(
			connectivity, n_local_nodes,
			values, history, model, dt ); 
	std::vector< std::vector<int> > expected_buflengths = std::vector< std::vector<int> >(4);
	static const int arr0[] = {3, 4, 5, 4, 4, 5, 5}; 
	expected_buflengths[0] = std::vector<int>(arr0, arr0 + sizeof(arr0) / sizeof(arr0[0]) );
	static const int arr1[] = {3, 5, 6, 6, 5, 3, 6, 6}; 
	expected_buflengths[1] = std::vector<int>(arr1, arr1 + sizeof(arr1) / sizeof(arr1[0]) );
	static const int arr2[] = {2, 6, 3, 6, 6, 4}; 
	expected_buflengths[2] = std::vector<int>(arr2, arr2 + sizeof(arr2) / sizeof(arr2[0]) );
	static const int arr3[] = {6, 4, 5, 4, 3, 4, 6}; 
	expected_buflengths[3] = std::vector<int>(arr3, arr3 + sizeof(arr3) / sizeof(arr3[0]) );
	for (int i = 0; i < size; i++) {
		if(task_id == i){
			INFO( "process: "<< i)
			CHECK(initial_conditions.size() == expected_buflengths[i].size());
			for (global_history_type::size_type j = 0; j < initial_conditions.size(); j++) {
				INFO( "buffer: "<< j);
				INFO("buffer length: " << initial_conditions[j]->get_length());
				CHECK(initial_conditions[j]->get_length() == expected_buflengths[i][j]);
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}


}

TEST_CASE("Parallel integration time stepping", "[mpi euler euler-maruyama]")
{
	int task_id, size;
    MPI_Comm_rank (MPI_COMM_WORLD, &task_id);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    CHECK(size == 4); //required for partitioned test inputs
    CHECK(task_id < 4); //sanity check...

	unsigned long n_local_nodes;
	if(task_id == 1){
		n_local_nodes = 5;
	}else{
		n_local_nodes =4;
	}
	double dt = 0.2;

	generic_2d_oscillator *model = new generic_2d_oscillator();

	// all disconnected, only 3->4 with 0.1 delay (between proc 0 and 1)
	global_connectivity_type connectivity = global_connectivity_type(n_local_nodes); 
	neighbor_map_type recv_node_ids; 
	neighbor_map_type send_node_ids;
	if(task_id == 0){ 
		std::vector<unsigned long> send_ids;
		send_ids.push_back(3);
		send_node_ids.push_back(std::make_pair(1, send_ids));
	} else if(task_id == 1){
		std::vector<unsigned long> recv_ids;
		recv_ids.push_back(4);
		recv_node_ids.push_back(std::make_pair(0,recv_ids));
		local_connectivity_type connections = local_connectivity_type();
		connection conn = {
			4, // from
			0.1, // delay
			1.0 // weight
		};
		connections.push_back(conn);
		connectivity[0] = connections; 

	}
	
	linear_coupling *coupling = new linear_coupling();

	lint_history_factory* history = new lint_history_factory();
	local_state_type values = local_state_type(model->n_vars(),1.6);
	global_history_type initial_conditions = mpi_integrator::constant_initial_conditions(
			connectivity, n_local_nodes,
			values, history, model, dt ); 

	SECTION("generating initial conditions"){
		for(unsigned long i = 0; i< n_local_nodes; i++){
			if(i == 3 && task_id == 0){
				CHECK(initial_conditions[i]->get_length() == 2);
			}else{
				CHECK(initial_conditions[i]->get_length() == 1);
			}
		}
	}

	SECTION("integration"){
		raw_observer *observer = new raw_observer(n_local_nodes);

		mpi_euler integrator = mpi_euler(
				model, coupling, connectivity, initial_conditions, observer,
				dt, recv_node_ids, send_node_ids);

		integrator(5);
		if (task_id == 0) {
			CHECK(observer->get_solution()[0][0].second[0] ==Approx(1.620736) );
			CHECK(observer->get_solution()[0][0].second[1] ==Approx(1.5216) );
			CHECK(observer->get_solution()[0][4].second[0] ==Approx(1.70200895) );
			CHECK(observer->get_solution()[0][4].second[1] ==Approx(1.20292698) );
		}
		for(unsigned long i = 0; i< n_local_nodes; i++){
			if(i == 0 && task_id ==1){
				CHECK(observer->get_solution()[i][4].second[0] != Approx(observer->get_solution()[0][4].second[0]));
				CHECK(observer->get_solution()[i][4].second[1] != Approx(observer->get_solution()[0][4].second[1]));
			}else{
				CHECK(observer->get_solution()[i][4].second[0] == Approx(observer->get_solution()[0][4].second[0]));
				CHECK(observer->get_solution()[i][4].second[1] == Approx(observer->get_solution()[0][4].second[1]));
			}
		}
	}


}
