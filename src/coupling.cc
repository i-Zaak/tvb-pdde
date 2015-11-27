linear_coupling::linear_coupling(double a, double b)
{
	this->a = a;
	this->b = b;
}

void linear_coupling::operator()(	const global_connectivity_type &connectivity,
									const global_state_type &global_state,
									global_coupling_type &coupling)
{
	assert(global_state.size() == coupling.size() == connectivity.size());

	for(global_connectivity:size_type i=0; i < connectivity.size(); i++){
		int from = i;

		for(local_connectivity_type:size_type j=0; j < connectivity[i].size(); j++){
			int to = connectivity[i][j].to;
			double delay = connectivity[i][j].delay;

			local_state_type remote_state = global_state[to].get_values(delay);
			for(local_state_type:size_type dim=0; dim < node_coupling.size(); dim++){
				// todo sane vector type?
				coupling[i][dim] += this->a * remote_state[dim];
			}
		}

		for(local_state_type:size_type dim=0; dim < node_coupling.size(); dim++){
			// todo sane vector type?
			coupling[i][dim] += this->b;
		}
	}

}
