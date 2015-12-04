#include "catch.hpp"
#include "connectivity.h"

TEST_CASE("Reading connectivity from .mtx file", "[connectivity io]"){
	global_connectivity_type connectivity = connectivity_from_mtx("data/conn500.mtx");
	REQUIRE(connectivity.size() == 561);
	REQUIRE(connectivity[0].size() == 5);
	REQUIRE(connectivity[0].size() == 5);
	REQUIRE(connectivity[560].size() == 18);
	REQUIRE(connectivity[560][17].from == 544);
	REQUIRE(connectivity[560][17].delay == Approx(9.57745504336958));

}
