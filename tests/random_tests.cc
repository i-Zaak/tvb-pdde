#include "catch.hpp"
#include "random.h"
#include <numeric>



TEST_CASE("Boost noise generator", "[noise]"){
	boost_prng* generator = new boost_prng();
	unsigned int seed = 42;
	generator->seed(42);
	double t1 = generator->normal();
	double t2 = generator->normal();
	REQUIRE( t1 != Approx(t2));
	generator->seed(42);
	double t3 = generator->normal();
	REQUIRE( t1 == Approx(t3));

	std::vector<double> rands = std::vector<double>(1000);
	generator->fill_normal(rands);

	double sum = std::accumulate(rands.begin(), rands.end(), 0.0);
	double mean = sum / rands.size();
	REQUIRE(mean == Approx(0.0).epsilon(0.1));
	
	double squared_sum = std::inner_product(	rands.begin(), rands.end(), 
												rands.begin(), 0.0);
	double stdev = std::sqrt(squared_sum / rands.size() - mean * mean);
	REQUIRE(stdev == Approx(1.0).epsilon(0.01));
}
