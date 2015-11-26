// state variables of single population
typedef local_state_type std::vector<double>; 

// coupling variables of single population
typedef local_coupling_type std::vector<double>; 

// coupling variables of all populations
typedef global_coupling_type std::vector< std::vector<double> >;

// connectivity pattern between populations: to x from (sparse)
typedef connectivity_type std::vector<double> 

// history buffers for state variables for all populations
typedef global_history_type std::vector< history_buffers >; 

/**
 Provides `dfun` function to evaluate the right-hand side of the model for
 single population. Also holds the constants. Sizes of the arrays are fixed 
 for given model. 

 `phi`			local set of state variables
 `coupling` 	contributions from other populations: can have multiple
 				components (coupling variables)
 `dphidt`		result of the same size as `phi`


 */
class population_model
{
	public:
		virtual void operator()(	const local_state_type &phi, 
									const local_coupling_type &coupling, 
									local_state_type &dphidt)=0;
};

class generic_oscillator:population_model
{
	private:
		double tau = 1.0;
		double a = -2.0;
		double b = -10.0;
		double c = 0.0;
		double I = 0.0;
		double d = 0.02;
		double e = 3.0;
		double f = 1.0;
		double g = 0.0;
		double alpha = 1.0;
		double beta = 1.0;
		double gamma = 1.0;
	public:
		void operator()(	const local_state_type &phi, 
							const local_coupling_type &coupling, 
							local_state_type &dphidt);
}

/**
 Computes the remote contributions between populations. 
 */
class population_coupling
{
	public:
		virtual void operator()(	const connectivity_type &connectivity,
									const global_state_type &state,
									global_coupling_type &coupling)=0;
};

class linear_coupling:population_coupling
{
	private:
		double a = 1.0;
		double b = 0.0;
	public:
		virtual void operator()(	const connectivity_type &connectivity,
									const global_state_type &state,
									global_coupling_type &coupling)=0;
}


/**
 Performs the main integration loop for every node:
  * evaluate coupling
  * perform integration step
  * update history

 Also setups, holds and tears down the integration data structures. 
 */
class integrator 
{
	protected:
		model_coupling coupling;
		integration_scheme scheme;
		
	public:
		integrator(	population_model model,
					model_coupling coupling,
					integration_scheme scheme);
};

