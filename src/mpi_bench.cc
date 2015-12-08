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
	std::string filename = argv[1];
	double dt = atof(argv[2]);
	int n_steps = atoi(argv[3]);

	MPI_Init(&argc,&argv);

	global_connectivity_type connectivity = connectivity_from_mtx(filename);
	lint_history_factory* history = new lint_history_factory();
	generic_2d_oscillator *model = new generic_2d_oscillator();
	local_state_type values = local_state_type(model->n_vars(),1.5);
	global_history_type initial_conditions = integrator::constant_initial_conditions(
			connectivity, values, history, model, dt ); 
	linear_coupling *coupling = new linear_coupling();
	raw_observer *observer = new raw_observer(connectivity.size());
	euler_deterministic integrator = euler_deterministic(
			model, coupling, connectivity, initial_conditions, observer,
			dt);

	integrator(n_steps);
	//TODO save results
	
	MPI_Finalize();
	return 0;
}