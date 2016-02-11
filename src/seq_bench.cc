#include<iostream>
#include<string>

#include "common.h"
#include "connectivity.h"
#include "coupling.h"
#include "history.h"
#include "integrator.h"
#include "model.h"
#include "observer.h"



int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cout << "usage: " << argv[0] << " connectivity_file.mtx dt n_steps" << std::endl;
		return 1;
	}
	double dt = atof(argv[2]);
	unsigned long n_steps = atoi(argv[3]);
	std::ifstream conn_file(argv[1]);
	if(!conn_file.is_open()) {
		std::cout << "could not open " << argv[1] << std::endl;
		return 2;
	}
	global_connectivities_type connectivities = global_connectivities_type(2);
	global_connectivity_type connectivity = connectivity_from_mtx(conn_file);
	connectivities[0] = connectivity;
	global_connectivity_type emptyconn = global_connectivity_type(1);
	connectivities[1] = emptyconn;

	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),1.5);
	global_histories_type initial_conditions = global_histories_type(2);
	initial_conditions[0] = integrator::constant_initial_conditions(
			connectivity,values, history, model, dt ); 
	initial_conditions[1] = new empty_history(model->n_vars());

	linear_coupling *coupling = new linear_coupling();
	raw_observer *observer = new raw_observer(connectivity.size());
	euler integrator = euler(
			model, coupling, connectivities, initial_conditions, observer,
			dt);

	integrator(n_steps);
	//TODO save results
	return 0;
}
