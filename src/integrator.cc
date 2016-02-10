#include "integrator.h"
#include "math.h"

integrator::integrator(	population_model *model,
						population_coupling *coupling,
						const std::vector<global_connectivity_type> &connectivities,
						const std::vector<global_history*> &initial_conditions,
					solution_observer *observer,
						double dt)
{
	this->model = model;
	this->coupling = coupling;
	this->connectivities = connectivities;
	this->histories = initial_conditions;
	assert(initial_conditions.size()>0);
	this->scheme_history = histories[0];
	this->observer = observer;
	this->dt = dt;
	this->n_nodes = initial_conditions[0]->n_nodes();
}

void integrator::operator()(unsigned long n_steps)
{
	for(unsigned long i=0;i<n_steps;i++) {
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
	for(std::size_t i=0; i < this->histories.size();i++){
		this->histories[i]->push_state(global_state);
	}
	
}

/**
 Wraps the coupling and local model evaluation to one reasonable function call.
 To be used in the integrator scheme instead of direct calls to model/coupling.
 */
void integrator::dfun_eval(	unsigned long node,
							const local_state_type phi, 
							double time_offset, 
							local_state_type &df,
							local_state_type &dg)
{
	local_coupling_type l_coupling = local_coupling_type(this->histories.size());
	for(std::size_t i=0; i < this->histories.size(); i++){
		std::size_t local_node = this->histories[i]->local_node_id(node);
		local_state_type lc = local_state_type(this->model->n_vars());
		(*this->coupling)( this->connectivities[i][local_node], 
						this->histories[i], 
						lc,
						time_offset);
		l_coupling[i] = lc;
	}
	(*this->model)(	phi,
					l_coupling,
					df,
					dg);
}

double euler::scheme(unsigned long node, local_state_type &new_state)
{
	local_state_type phi = this->scheme_history->get_buffers(node)->get_values_at(0); // current 
	local_state_type df = local_state_type(this->model->n_vars());
	local_state_type dg = local_state_type(this->model->n_vars()); // this gets ignored...
	
	this->dfun_eval(node, phi, 0.0, df, dg);
	for(local_state_type::size_type dim=0; dim<phi.size();dim++){
		new_state[dim] = phi[dim] + this->dt * df[dim]; 
	}

	return this->dt; //eqidistant timestepping here
}

euler_maruyama::euler_maruyama(		population_model *model,
									population_coupling *coupling,
									const std::vector<global_connectivity_type> &connectivities,
									const std::vector<global_history*> &initial_conditions,
									solution_observer *observer,
									rng *noise_generator,
									double dt
								):integrator( 	model, coupling, connectivities, 
												initial_conditions, observer,
												dt
								)
{
	this->noise_generator = noise_generator;
};


double euler_maruyama::scheme(unsigned long node, local_state_type &new_state)
{
	local_state_type phi = this->scheme_history->get_buffers(node)->get_values_at(0); // current 
	local_state_type df = local_state_type(this->model->n_vars());
	local_state_type dg = local_state_type(this->model->n_vars()); 

	local_state_type dw = local_state_type(this->model->n_vars());
	this->noise_generator->fill_normal(dw);
	
	this->dfun_eval(node, phi, 0.0, df, dg);
	double sqrtdt = sqrt(this->dt); // needed for \delta W
	for(local_state_type::size_type dim=0; dim<phi.size();dim++){
		new_state[dim] = phi[dim] + this->dt * df[dim] + sqrtdt * dg[dim] * dw[dim]; 
	}

	return this->dt; //eqidistant timestepping here
}

global_histories_type integrator::constant_initial_conditions(
		const global_connectivities_type &connectivities,
		//unsigned long n_nodes,
		const local_state_type &values,
		history_factory* history,
		population_model* model,
		double dt)
{
	global_histories_type initial_conditions = global_histories_type();
	if(connectivities.size() == 0) return initial_conditions;


	for (std::size_t i = 0; i < connectivities.size(); i++) {
		std::size_t n_nodes = connectivities[i].size();

		std::vector< history_buffers* > buffers = std::vector< history_buffers* >(n_nodes);
		
		// determine buffer lengths
		std::vector<double> max_delays = std::vector<double>(n_nodes,0.0);
		for(std::size_t j=0; j < connectivities[i].size(); j++){
			for(std::size_t k=0; k< connectivities[i][j].size(); k++)	{
				connection conn = connectivities[i][j][k];
				if ( max_delays[conn.from] < conn.delay) {
					max_delays[conn.from] = conn.delay;
				}
			}
		}

		// create buffers and fill with constant state
		for(std::size_t j=0; j < buffers.size(); j++){
			unsigned long length = ceil(max_delays[j] / dt)+1;
			buffers[j] = history->create_history(length, dt, model->n_vars());
			for(unsigned long k = 0; k < length; k++) {
				buffers[j]->add_state(values);
			}
		}
		global_history *history = new global_history(buffers);
		initial_conditions.push_back(history);
	}
	return initial_conditions;
}

