#include "catch.hpp"
#include "integrator.h"

TEST_CASE("Integration time stepping", "[euler method]")
{
	unsigned long n_nodes = 13;
	double dt = 0.1;
	generic_2d_oscillator model = generic_2d_oscillator();
	global_connectivity_type connectivity = global_connectivity_type(n_nodes);

	linear_coupling coupling = linear_coupling,
								global_connectivity_type connectivity,
								global_history_type initial_conditions,
								unsigned long n_nodes,
								double dt

	euler_deterministic integrator = euler_deterministic(model, coupling, connectivity, initial_conditions, n_nodes, dt);

	integrator(5);
}
