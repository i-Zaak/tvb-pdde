#include "catch.hpp"
#include "integrator.h"

//TODO refactor a lot of the initialization to constructors/configuration...
TEST_CASE("Integration time stepping", "[euler method]")
{
	unsigned long n_nodes = 13;
	double dt = 0.2;

	generic_2d_oscillator *model = new generic_2d_oscillator();

	// all disconnected, only 2->3 with 0.1 delay
	global_connectivity_type connectivity = global_connectivity_type(n_nodes); 
	local_connectivity_type connections = local_connectivity_type();
	connection conn = {
		2, // from
		0.1, // delay
		1.0 // weight
	};
	connections.push_back(conn);
	connectivity[3] = connections; 


	linear_coupling *coupling = new linear_coupling();
	
	lint_history_factory* history = new lint_history_factory();
	local_state_type values = local_state_type(model->n_vars(),1.6);
	global_history_type initial_conditions = integrator::constant_initial_conditions(
			connectivity, connectivity.size(), values, history, model, dt ); 
	SECTION("generating initial conditions"){
		for(unsigned long i = 0; i< n_nodes; i++){
			if(i == 2){
				REQUIRE(initial_conditions[i]->get_length() == 2);
			}else{
				REQUIRE(initial_conditions[i]->get_length() == 1);
			}
		}

	}
	SECTION("integration"){
		raw_observer *observer = new raw_observer(n_nodes); //this could be in the integrator constructor


		euler integrator = euler(
				model, coupling, connectivity, initial_conditions, observer,
				dt);

		integrator(5);
		REQUIRE(observer->get_solution()[0][0].second[0] ==Approx(1.620736) );
		REQUIRE(observer->get_solution()[0][0].second[1] ==Approx(1.5216) );
		REQUIRE(observer->get_solution()[0][4].second[0] ==Approx(1.70200895) );
		REQUIRE(observer->get_solution()[0][4].second[1] ==Approx(1.20292698) );
		for(unsigned long i = 0; i< n_nodes; i++){
			if(i == 3){
				REQUIRE(observer->get_solution()[i][4].second[0] != Approx(observer->get_solution()[0][4].second[0]));
				REQUIRE(observer->get_solution()[i][4].second[1] != Approx(observer->get_solution()[0][4].second[1]));
			}else{
				REQUIRE(observer->get_solution()[i][4].second[0] == Approx(observer->get_solution()[0][4].second[0]));
				REQUIRE(observer->get_solution()[i][4].second[1] == Approx(observer->get_solution()[0][4].second[1]));
			}
		}
	}

}

TEST_CASE("Initial conditions from connectivity" "[integrator]"){
	// see sets_init.py for data generation
	global_connectivity_type connectivity = connectivity_from_mtx("data/test_init.mtx");
	REQUIRE(connectivity.size() == 20);

	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),42.0);
	double dt=0.2;
	global_history_type initial_conditions = integrator::constant_initial_conditions(
			connectivity, connectivity.size(),values, history, model, dt ); 
	static const int arr[] = {3, 4, 5, 2, 6, 3, 6, 6, 4, 5, 6, 6, 6, 6, 4, 5, 4, 5, 4, 4}; 
	std::vector<int> expected_buflengths(arr, arr + sizeof(arr) / sizeof(arr[0]) );

	for(global_history_type::size_type i = 0; i < initial_conditions.size(); i++) {
		REQUIRE(initial_conditions[i]->get_length() == expected_buflengths[i]);
	}

}

