#include "catch.hpp"

#include "history.h"
#include<vector>

TEST_CASE( "Pushing to history", "[history]" ) {
	int n_vars = 3;
	lint_history h_bufs = lint_history(4,1.0,n_vars);
	for (int i = 0; i < 8; i++) {
		std::vector<double> vals = std::vector<double>(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals[j] = i+j;
		}
		h_bufs.add_state(vals);
	}
	
	REQUIRE( h_bufs.get_value_at(0,0) == Approx(7) );
	REQUIRE( h_bufs.get_value_at(0,1) == Approx(8) );
	REQUIRE( h_bufs.get_value_at(0,2) == Approx(9) );
	REQUIRE( h_bufs.get_value_at(1,2) == Approx(8) );
	REQUIRE( h_bufs.get_value_at(2,2) == Approx(7) );
	REQUIRE( h_bufs.get_value_at(3,2) == Approx(6) );
}

TEST_CASE( "History interpolation", "[history]" ) {
	int n_vars=3;
	lint_history h_bufs = lint_history(3, 0.2,n_vars);
	for (int i = 0; i < 10; i++) {
		std::vector<double> vals = std::vector<double>(n_vars);
		for (int j = 0; j < n_vars;j++){
			vals[j] = i+j;
		}
		h_bufs.add_state(vals);
	}
	REQUIRE( h_bufs.get_value(0.0,0) == Approx(9) );
	REQUIRE( h_bufs.get_value(0.0,1) == Approx(10) );
	REQUIRE( h_bufs.get_value(0.0,2) == Approx(11) );

	REQUIRE( h_bufs.get_value(0.2,0) == Approx(8) );
	REQUIRE( h_bufs.get_value(0.2,1) == Approx(9) );
	REQUIRE( h_bufs.get_value(0.2,2) == Approx(10) );

	REQUIRE( h_bufs.get_value(0.3,0) == Approx(7.5) );
	REQUIRE( h_bufs.get_value(0.3,1) == Approx(8.5) );
	REQUIRE( h_bufs.get_value(0.3,2) == Approx(9.5) );
	
	static const double arr[] = {7.5,8.5,9.5};
	local_state_type state(arr, arr + sizeof(arr) / sizeof(arr[0]) );
	REQUIRE( h_bufs.get_values(0.3) == state );
}	
