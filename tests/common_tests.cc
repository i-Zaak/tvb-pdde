#include "catch.hpp"
#include "common.h"

TEST_CASE("Common utilities and functions", "[common]"){
	double **a = alloc_2d_array(3,10);
	a[2][9] = 3.14;
	REQUIRE(a[2][9] == Approx(3.14)); // just for seqfaults
}
