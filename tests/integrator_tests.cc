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

	global_history_type initial_conditions = global_history_type(n_nodes); // this should be possible to generate from connectivity...
	for(global_history_type::size_type i=0; i < initial_conditions.size(); i++){
		local_state_type state = local_state_type(model->n_vars(),1.6); // just some initial state
		if(i==2){
			initial_conditions[i] = new lint_history(2,dt,model->n_vars());
			initial_conditions[i]->add_state(state);
			initial_conditions[i]->add_state(state);
		} else {
			initial_conditions[i] = new lint_history(1,dt,model->n_vars());
			initial_conditions[i]->add_state(state);
		}
	}

	raw_observer *observer = new raw_observer(n_nodes); //this could be in the integrator constructor


	euler_deterministic integrator = euler_deterministic(
			model, coupling, connectivity, initial_conditions, observer,
			n_nodes, dt);

	integrator(5);
	REQUIRE(observer->get_solution()[0][0].second[0] ==Approx(1.620736) );
	REQUIRE(observer->get_solution()[0][0].second[1] ==Approx(1.5216) );
	REQUIRE(observer->get_solution()[0][4].second[0] ==Approx(1.70200895) );
	REQUIRE(observer->get_solution()[0][4].second[1] ==Approx(1.20292698) );
	for(unsigned long i = 0; i< n_nodes; i++){
		if(i == 3){
			REQUIRE(observer->get_solution()[i][4].second[0] != observer->get_solution()[0][4].second[0]);
			REQUIRE(observer->get_solution()[i][4].second[1] != observer->get_solution()[0][4].second[1]);
		}else{
			REQUIRE(observer->get_solution()[i][4].second[0] == observer->get_solution()[0][4].second[0]);
			REQUIRE(observer->get_solution()[i][4].second[1] == observer->get_solution()[0][4].second[1]);
		}
	}

}
