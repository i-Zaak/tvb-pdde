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
	
	REQUIRE( h_bufs.get_value(0,0) == Approx(6) );
	REQUIRE( h_bufs.get_value(1,0) == Approx(4) );
	REQUIRE( h_bufs.get_value(2,0) == Approx(1) );
}
