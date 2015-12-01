#include "catch.hpp"
#include "observer.h"

TEST_CASE("Observer constructor", "[observer]")
{

	raw_observer observer1 = raw_observer(15);
	raw_observer observer2 = raw_observer(15, 40);
	

	REQUIRE(observer1.get_solution().size() == 15);
	for(global_solution_type::size_type i=0; i< observer1.get_solution().size(); i++){
		REQUIRE(observer1.get_solution()[i].capacity() == 0);
	}
	
	REQUIRE(observer2.get_solution().size() == 15);
	for(global_solution_type::size_type i=0; i< observer2.get_solution().size(); i++){
		REQUIRE(observer2.get_solution()[i].capacity() == 40);
	}
}

/*
TEST_CASE("Observer solution recording", "[observer]")
{
	raw_observer observer = raw_observer
}*/
