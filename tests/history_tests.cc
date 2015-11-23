#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "history.h"
#include<vector>

TEST_CASE( "Pushing to history", "[history]" ) {
	std::vector<int> buf_lengths = std::vector<int>(3);
	buf_lengths[0] = 2;
	buf_lengths[1] = 4;
	buf_lengths[2] = 7;
	history_buffers h_bufs = history_buffers(buf_lengths);
	for (int i = 0; i < 8; i++) {
		std::vector<double> vals = std::vector<double>(3);
		std::fill( vals.begin(), vals.end(), double(i));
		h_bufs.add_values(vals);
	}
	
	REQUIRE( h_bufs.get_value(0,1) == Approx(6) );
	REQUIRE( h_bufs.get_value(1,3) == Approx(4) );
	REQUIRE( h_bufs.get_value(2,6) == Approx(1) );
	REQUIRE( h_bufs.get_value(0,0) == Approx(7) );
	REQUIRE( h_bufs.get_value(1,0) == Approx(7) );
	REQUIRE( h_bufs.get_value(2,0) == Approx(7) );
}

TEST_CASE( "History interpolation", "[history]" ) {
	std::vector<int> buf_lengths = std::vector<int>(3);
	buf_lengths[0] = 6;
	buf_lengths[1] = 2;
	buf_lengths[2] = 9;
	interpolated_history h_bufs = interpolated_history(buf_lengths, 0.2);
	for (int i = 0; i < 10; i++) {
		std::vector<double> vals = std::vector<double>(3);
		std::fill( vals.begin(), vals.end(), double(i));
		h_bufs.add_values(vals);
	}
	REQUIRE( h_bufs.get_value_t(0,2.0) == Approx(9) );
	REQUIRE( h_bufs.get_value_t(1,2.0) == Approx(9) );
	REQUIRE( h_bufs.get_value_t(2,2.0) == Approx(9) );

	REQUIRE( h_bufs.get_value_t(0,1.8) == Approx(8) );
	REQUIRE( h_bufs.get_value_t(1,1.8) == Approx(8) );
	REQUIRE( h_bufs.get_value_t(2,1.8) == Approx(8) );

	REQUIRE( h_bufs.get_value_t(0,1.9) == Approx(8.5) );
	REQUIRE( h_bufs.get_value_t(1,1.9) == Approx(8.5) );
	REQUIRE( h_bufs.get_value_t(2,1.9) == Approx(8.5) );
}	
