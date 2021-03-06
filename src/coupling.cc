#include "coupling.h"
linear_coupling::linear_coupling(double a, double b)
{
	this->a = a;
	this->b = b;
}

void linear_coupling::operator()(	const local_connectivity_type &connectivity,
									const global_history *global_history,
									local_state_type &coupling,
									double time_offset)
{
	for(	local_connectivity_type::const_iterator connection = connectivity.begin();
			connection != connectivity.end(); connection++){
		unsigned long from = connection->from;
		double delay = connection->delay - time_offset;

		local_state_type remote_state = global_history->get_buffers(from)->get_values(delay);
		for(local_state_type::size_type dim=0; dim < coupling.size(); dim++){
			// todo sane vector type?
			coupling[dim] += this->a * remote_state[dim];
		}
	}

	for(local_state_type::size_type dim=0; dim < coupling.size(); dim++){
		// todo sane vector type?
		coupling[dim] += this->b;
	}

}
