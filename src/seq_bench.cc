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
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " connectivity_file.mtx" << std::endl;
		return 1;
	}
	std::string filename = argv[1];

	global_connectivity_type connectivity = connectivity_from_mtx(filename);
	generic_2d_oscillator *model = new generic_2d_oscillator();
	raw_observer *observer = new raw_observer(connectivity.size());
		
	euler_deterministic integrator = euler_deterministic(
			model, coupling, connectivity, initial_conditions, observer,
			n_nodes, dt);

	return 0;
}
