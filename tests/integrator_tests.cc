#include "catch.hpp"
#include "integrator.h"

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


	euler_deterministic integrator = euler_deterministic(model, coupling, connectivity, initial_conditions, n_nodes, dt);

	integrator(5);

	for(unsigned long i = 0; i< n_nodes; i++){
		if(i == 3){
			REQUIRE(integrator.history[i]->get_value_at(0,0) != Approx(integrator.history[0]->get_value_at(0,0)));
			//REQUIRE(integrator.history[i]->buffer[0][1] != Approx(integrator.history[0]->buffer[0][1]));
		}else{
			//REQUIRE(integrator.history[i]->buffer[0][0] == Approx(integrator.history[0]->buffer[0][0]));
			//REQUIRE(integrator.history[i]>buffer[0][1] == Approx(integrator.history[0]->buffer[0][1]));
		}
	}
}
