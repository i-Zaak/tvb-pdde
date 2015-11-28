#include "catch.hpp"
#include "coupling.h"

TEST_CASE("Evaluating coupling", "[linear coupling]"){
	int n_vars = 3;

	local_connectivity_type connectivity = local_connectivity_type();
	connection conn1 = {
		0, // from
		0.7, // delay
		1.0 // weight
	};
	connectivity.push_back(conn1);
	connection conn2 = {
		2, // from
		1.6, // delay
		1.0 // weight
	};
	connectivity.push_back(conn2);


	global_history_type global_history = global_history_type();	
	lint_history* hist0 = new lint_history(3,0.5,n_vars);
	for (int i = 0; i < 5; i++) {
		local_state_type vals0 = local_state_type(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals0[j] = i+j;
		}
		hist0->add_state(vals0);
	}
	lint_history *hist2 = new lint_history(5,0.5,n_vars);
	for (int i = 10; i < 15; i++) {
		local_state_type vals2 = local_state_type(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals2[j] = i+j;
		}
		hist2->add_state(vals2);
	}
	lint_history *hist1 = new lint_history(1,0.5,n_vars);
	local_state_type vals1 = local_state_type(n_vars,50.0);
	hist1->add_state(vals1);

	global_history.push_back(hist0);
	global_history.push_back(hist1);
	global_history.push_back(hist2);


	local_coupling_type coupling = local_coupling_type(n_vars,0.0);
	
	linear_coupling coupling_function = linear_coupling();

	coupling_function(connectivity, global_history, coupling);
	
	static const double arr[] = {13.4,15.4,17.4};
	local_coupling_type expected_coupling(arr, arr + sizeof(arr) / sizeof(arr[0]) );

	REQUIRE( coupling[0] == Approx(expected_coupling[0]) );
	REQUIRE( coupling[1] == Approx(expected_coupling[1]) );
	REQUIRE( coupling[2] == Approx(expected_coupling[2]) );
}
