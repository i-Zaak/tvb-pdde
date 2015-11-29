#include "integrator.h"

integrator(	population_model model,
			model_coupling coupling,
			global_connectivity_type connectivity,
			global_history_type initial_conditions,
			double dt)
{
	this->model = model;
	this->coupling = coupling;
	this->connectivity = connectivity;
	this->history = initial_conditions;
	this->dt = dt;
}

void integrator::operator(unsigned int n_steps)
{
	for(unsigned int i=0;i<n_steps;i++) {
		this->step();
		//TODO push the state to observer
	}
}

void integrator::step()
{
	for(global_state_type::size_type i=0; i< this->global_state; i++){
		//scheme
	}
	for(global_state_type::size_type i=0; i< this->global_state; i++){
		//update state
		//update history
	}

}

/**
 Wraps the coupling and local model evaluation to one reasonable function call.
 To be used in the integrator scheme.
 */
void integrator::dfun_eval(	unsigned int node,
							const local_state_type phi, 
							double time_offset, 
							local_state_type &dphidt)
{
	local_coupling_type l_coupling = local_coupling_type(this->model.n_vars());
	this->coupling( this->connectivity[node], 
					this->history, 
					delay_offset,
					l_coupling);

	this->model(	phi,
					l_coupling,
					dphidt);
}

void euler_deterministic::scheme(unsigned int node, local_state_type &new_state)
{
	local_state_type phi = this->history[node]->get_values_at(0); // current 
	local_state_type dphidt = local_state_type(this->model.n_vars());
	this->dfun_eval(node, phi, 0.0, dphidt);
	for(local_state_type::size_type dim=0; dim<phi.size();dim++){
		//X + self.dt * (self.dX + stimulus)
		new_state[dim] = phi + this->dt * dphidt[dim];
	}
}
