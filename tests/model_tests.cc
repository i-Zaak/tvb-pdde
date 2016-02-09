#include "catch.hpp"
#include "model.h"

TEST_CASE( "Model evaluation", "[generic 2d oscillator]"){
	// this works only for the default set of parameters, check against TVB
	generic_2d_oscillator g2d = generic_2d_oscillator();
	unsigned int n_vars = g2d.n_vars();
	
	local_state_type phi = local_state_type(n_vars);
	phi[0] = 0.97607082;  // see TVB tests
	phi[1] = -0.03384097; // see TVB tests
	local_state_type df = local_state_type(n_vars);
	local_state_type dg = local_state_type(n_vars);
	local_coupling_type coupling = local_coupling_type();
	coupling.push_back(local_state_type(n_vars, 0.0)); //surface
	coupling.push_back(local_state_type(n_vars, 0.0)); //regional
	

	g2d(phi,coupling,df,dg); // zero coupling
	REQUIRE( df[0] == Approx(0.0378877) );
	REQUIRE( df[1] == Approx(-0.23453734) );
	REQUIRE( dg[0] == Approx(0.001) ); //constant function
	REQUIRE( dg[1] == Approx(0.001) );

	coupling.clear();
	coupling.push_back(phi); //surface
	coupling.push_back(phi); //regional

	g2d(phi,coupling,df,dg); // coupled with one identical oscillator
	REQUIRE( df[0] == Approx(0.05740912) );
	REQUIRE( df[1] == Approx(-0.23453734) );
	REQUIRE( dg[0] == Approx(0.001) ); //constant function
	REQUIRE( dg[1] == Approx(0.001) );
}
