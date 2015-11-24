/**
 Provides `dfun` function to evaluate the derivative. Note that the *local
 coupling* is here just another coupling variable.
 */
class population_model
{
	public:
		virtual void operator()(const &state_variables, 
							std::vector<double> &coupling, 
							std::vector<double> &new_state)=0;
}

/**
 Performs the integration step.
 */
class integration_scheme
{
	private:
		population_model model;
	public:
		integration_scheme( population_model m ):model(m);
		virtual void step(	std::vector<double> &state_variables, 							
							std::vector<double> &coupling, 
							std::vector<double> &new_state)=0;
}

/**
 Performs the main integration loop for every node:
  * evaluate coupling
  * call integrator
  * update history

 Also setups, holds and tears down the integration data structures. 
 */
class integrator 
{
	private:
		model_coupling coupling;
		integration_scheme scheme;
	public:
		integrator(	population_model model,
					model_coupling coupling,
					integration_scheme scheme);
}

/**
 Computes the remote contributions.
 */
class model_coupling
{
	public:
}

