typedef local_state_type boost::array<double>;
typedef local_coupling_type boost::array<double>;
typedef global_state_type std::vector< history_buffers >;

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
}

