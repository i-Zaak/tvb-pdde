#include "catch.hpp"
#include "model.h"

TEST_CASE( "Model evaluation", "[generic 2d oscillator]"){
	// this works only for the default set of parameters, check against TVB
	generic_2d_oscillator g2d = generic_2d_oscillator();
	unsigned int n_vars = g2d.n_vars();
	
	local_state_type phi = local_state_type(n_vars);
	phi[0] = 0.97607082;  // see TVB tests
	phi[1] = -0.03384097; // see TVB tests
	local_state_type dphidt = local_state_type(n_vars);
	local_coupling_type coupling = local_coupling_type(n_vars, 0.0);

	g2d(phi,coupling,dphidt); // zero coupling
	REQUIRE( dphidt[0] == Approx(0.0378877) );
	REQUIRE( dphidt[1] == Approx(-0.23453734) );


	g2d(phi,phi,dphidt); // coupled with one identical oscillator
	REQUIRE( dphidt[0] == Approx(0.05740912) );
	REQUIRE( dphidt[1] == Approx(-0.23453734) );
}
