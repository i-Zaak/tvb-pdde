#include "observer.h"

raw_observer::raw_observer(unsigned long nodes, unsigned long steps)
{
	this->observation = global_solution_type(nodes);
	if (steps >0){
		for(	global_solution_type::size_type i=0; 
				i < this->observation.size();
				i++){
			this->observation[i].reserve(steps); 
		}
	}
}

void raw_observer::operator()(unsigned long node, local_state_type &state, double time )
{
	this->observation[node].push_back( std::make_pair(time, state) );
}

const global_solution_type &raw_observer::get_solution( )
{
	return this->observation;
}

