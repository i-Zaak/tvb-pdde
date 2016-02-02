#include "integrator.h"
#include "math.h"

integrator::integrator(	population_model *model,
						population_coupling *coupling,
						const global_connectivity_type &connectivity,
						const global_history_type &initial_conditions,
					solution_observer *observer,
						double dt)
{
	this->model = model;
	this->coupling = coupling;
	this->connectivity = connectivity;
	this->history = initial_conditions;
	this->observer = observer;
	this->dt = dt;
	this->n_nodes = initial_conditions.size();
}

void integrator::operator()(unsigned int n_steps)
{
	for(unsigned int i=0;i<n_steps;i++) {
		step();
	}
}

void integrator::step()
{
	global_state_type global_state = global_state_type(this->n_nodes);
	local_state_type local_state = local_state_type(this->model->n_vars());

	// compute new state phi(t_{n+1})
	#pragma omp parallel for firstprivate(local_state)
	for(unsigned long node=0; node< this->n_nodes; node++){
		double time; //of the next step
		time = scheme(node, local_state); 
		global_state[node] = local_state;

		// observe and behold
		(*this->observer)(node, local_state, time);
	}

	// update history
	#pragma omp parallel for
	for(unsigned long node=0; node< this->n_nodes; node++){
		// this can be run only after all nodes are done
		this->history[node]->add_state(global_state[node]);
	}
	
}

/**
 Wraps the coupling and local model evaluation to one reasonable function call.
 To be used in the integrator scheme instead of direct calls to model/coupling.
 */
void integrator::dfun_eval(	unsigned int node,
							const local_state_type phi, 
							double time_offset, 
							local_state_type &dphidt)
{
	local_coupling_type l_coupling = local_coupling_type(this->model->n_vars());
	(*this->coupling)( this->connectivity[node], 
					this->history, 
					l_coupling,
					time_offset);

	(*this->model)(	phi,
					l_coupling,
					dphidt);
}

double euler::scheme(unsigned int node, local_state_type &new_state)
{
	local_state_type phi = this->history[node]->get_values_at(0); // current 
	local_state_type dphidt = local_state_type(this->model->n_vars());
	this->dfun_eval(node, phi, 0.0, dphidt);
	for(local_state_type::size_type dim=0; dim<phi.size();dim++){
		new_state[dim] = phi[dim] + this->dt * dphidt[dim];
	}

	return this->dt; //eqidistant timestepping here
}


global_history_type integrator::constant_initial_conditions(
		const global_connectivity_type &connectivity,
		unsigned long n_nodes,
		const local_state_type &values,
		history_factory* history,
		population_model* model,
		double dt)
{

	global_history_type initial_conditions = global_history_type(n_nodes);
	
	// determine buffer lengths
	std::vector<double> max_delays = std::vector<double>(n_nodes,0.0);
	for(global_connectivity_type::size_type i=0; i < connectivity.size(); i++){
		for(local_connectivity_type::size_type j=0; j< connectivity[i].size(); j++)	{
			connection conn = connectivity[i][j];
			if ( max_delays[conn.from] < conn.delay) {
				max_delays[conn.from] = conn.delay;
			}
		}
	}

	// create buffers and fill with constant state
	for(global_history_type::size_type i=0; i < initial_conditions.size(); i++){
		int length = ceil(max_delays[i] / dt)+1;
		initial_conditions[i] = history->create_history(length, dt, model->n_vars());
		for(int j = 0; j < length; j++) {
			initial_conditions[i]->add_state(values);
		}
	}

	return initial_conditions;
}

