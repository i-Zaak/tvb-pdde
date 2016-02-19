#ifndef COUPLING_H
#define COUPLING_H	

#include "common.h"
#include "history.h"
#include "connectivity.h"


/**
 Computes the remote contributions between populations. 
 */
class population_coupling
{
	//protected:
	//	global_connectivity_type connectivity;	
	public:
		//population_coupling(global_connectivity_type connectivity);
		virtual void operator()(	const local_connectivity_type &connectivity,
									const global_history *history,
									local_state_type &coupling,
									double time_offset)=0;
};

class linear_coupling : public population_coupling
{
	private:
		double a;
		double b;
	public:
		//linear_coupling(global_connectivity_type connectivity, double a=1.0, double b=0.0):population_coupling(connectivity);
		linear_coupling(double a=1.0, double b=0.0);
		void operator()(	const local_connectivity_type &connectivity,
									const global_history *state,
									local_state_type &coupling,
									double time_offset = 0.0
									);
};

#endif
