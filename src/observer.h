#ifndef OBSERVER_H
#define OBSERVER_H	

#include "common.h"

class solution_observer
{
	public:
		virtual void operator()(unsigned long node, local_state_type &state, double time )=0;
};

class raw_observer : public solution_observer
{
	private:
		global_solution_type observation;
	public:
		raw_observer(unsigned long nodes, unsigned long steps=0);
		void operator()(unsigned long node, local_state_type &state, double time );
		const global_solution_type &get_solution( );
};

#endif
