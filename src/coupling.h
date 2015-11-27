#include "common.h"

struct connection
{
	int from;
	int to;
	double delay;
	double weight;
}

/**
 Computes the remote contributions between populations. 
 */
class population_coupling
{
	protected:
		global_connectivity_type connectivity;	
	public:
		population_coupling(global_connectivity_type connectivity);
		virtual void operator()(	const global_connectivity_type &connectivity,
									const global_state_type &state,
									global_coupling_type &coupling)=0;
};

class linear_coupling:population_coupling
{
	private:
		double a = 1.0;
		double b = 0.0;
	public:
		linear_coupling(global_connectivity_type connectivity):population_coupling(connectivity){};
		void operator()(	const global_connectivity_type &connectivity,
									const global_state_type &state,
									global_coupling_type &coupling);
}
