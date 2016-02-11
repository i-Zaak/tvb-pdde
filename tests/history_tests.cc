#include "catch.hpp"

#include "history.h"
#include<vector>

TEST_CASE( "Pushing to history", "[history]" ) {
	int n_vars = 3;
	lint_history h_bufs = lint_history(4,1.0,n_vars);
	for (int i = 0; i < 8; i++) {
		std::vector<double> vals = std::vector<double>(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals[j] = i+j;
		}
		h_bufs.add_state(vals);
	}
	
	REQUIRE( h_bufs.get_value_at(0,0) == Approx(7) );
	REQUIRE( h_bufs.get_value_at(0,1) == Approx(8) );
	REQUIRE( h_bufs.get_value_at(0,2) == Approx(9) );
	REQUIRE( h_bufs.get_value_at(1,2) == Approx(8) );
	REQUIRE( h_bufs.get_value_at(2,2) == Approx(7) );
	REQUIRE( h_bufs.get_value_at(3,2) == Approx(6) );
}

TEST_CASE( "Pushing to global history", "[history]" ) {
	int n_vars = 2;
	int n_nodes = 11;

	std::vector< history_buffers* > hist = std::vector< history_buffers* >();
	for (int i = 0; i < n_nodes; i++) {
		hist.push_back( new lint_history(5,3.14,n_vars));
	}

	global_history ghist = global_history(hist);

	global_state_type state = global_state_type(n_nodes);
	for (int i = 0; i < n_nodes; i++) {
		state[i] = local_state_type(n_vars, double(i));
	}
	ghist.push_state(state);

	REQUIRE(ghist.n_nodes()==n_nodes);
	REQUIRE(ghist.local_node_id(7)==7);
	REQUIRE(ghist.get_buffers(5)->get_value_at(0,0) == Approx(5.0));
	REQUIRE(ghist.get_buffers(5)->get_value_at(0,1) == Approx(5.0));


}

TEST_CASE( "Pushing to aggregated global history", "[history]" ) {
	int n_vars = 4;
	int n_nodes = 18;
	int n_regs = 3;

	std::vector< history_buffers* > hist = std::vector< history_buffers* >();
	for (int i = 0; i < n_nodes; i++) {
		hist.push_back( new lint_history(7,0.4,n_vars));
	}

	std::vector< std::vector< std::size_t > > region_nodes(n_regs);
	std::vector< std::size_t > nodes_region(n_nodes);
	for(unsigned int i=0; i<n_regs; i++){
		for (unsigned int j = 0; j < 6; j++) {
			std::size_t node = 6*i+j;
			nodes_region[node] = i;
			region_nodes[i].push_back(node);
		}
	}

	scatter_gather_history ghist = scatter_gather_history(hist, region_nodes, nodes_region);

	global_state_type state = global_state_type(n_nodes);
	for (int i = 0; i < n_nodes; i++) {
		state[i] = local_state_type(n_vars, double(i));
	}
	ghist.push_state(state);

	REQUIRE(ghist.n_nodes()==n_nodes);
	REQUIRE(ghist.local_node_id(7)==1);
	REQUIRE(ghist.local_node_id(0)==0);
	REQUIRE(ghist.local_node_id(n_nodes-1)==2);
	REQUIRE(ghist.get_buffers(13)->get_value_at(0,0) == Approx(14.5));
	REQUIRE(ghist.get_buffers(13)->get_value_at(0,1) == Approx(14.5));
}

TEST_CASE( "History interpolation", "[history]" ) {
	int n_vars=3;
	lint_history h_bufs = lint_history(3, 0.2,n_vars);
	for (int i = 0; i < 10; i++) {
		std::vector<double> vals = std::vector<double>(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals[j] = i+j;
		}
		h_bufs.add_state(vals);
	}
	REQUIRE( h_bufs.get_value(0.0,0) == Approx(9) );
	REQUIRE( h_bufs.get_value(0.0,1) == Approx(10) );
	REQUIRE( h_bufs.get_value(0.0,2) == Approx(11) );

	REQUIRE( h_bufs.get_value(0.2,0) == Approx(8) );
	REQUIRE( h_bufs.get_value(0.2,1) == Approx(9) );
	REQUIRE( h_bufs.get_value(0.2,2) == Approx(10) );

	REQUIRE( h_bufs.get_value(0.3,0) == Approx(7.5) );
	REQUIRE( h_bufs.get_value(0.3,1) == Approx(8.5) );
	REQUIRE( h_bufs.get_value(0.3,2) == Approx(9.5) );
	
	static const double arr[] = {7.5,8.5,9.5};
	local_state_type state(arr, arr + sizeof(arr) / sizeof(arr[0]) );
	REQUIRE( h_bufs.get_values(0.3) == state );
}	
