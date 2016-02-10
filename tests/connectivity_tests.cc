#include "catch.hpp"
#include "connectivity.h"
#include <fstream>

TEST_CASE("Reading connectivity from .mtx file", "[connectivity io]"){
	std::ifstream infile("data/conn500.mtx");
	global_connectivity_type connectivity = connectivity_from_mtx(infile);
	REQUIRE(connectivity.size() == 561);
	REQUIRE(connectivity[0].size() == 5);
	REQUIRE(connectivity[0].size() == 5);
	REQUIRE(connectivity[560].size() == 18);
	REQUIRE(connectivity[560][17].from == 544);
	REQUIRE(connectivity[560][17].delay == Approx(9.57745504336958));

}

TEST_CASE("Reading distributed connectivity from .adj.n.m file", "[connectivity io mpi"){
	global_connectivity_type connectivity;
	neighbor_map_type recv_node_ids; 
	neighbor_map_type send_node_ids;
	
	std::ifstream part_file("data/conn500.adj.part.4.3");
	std::ifstream conn_file("data/conn500.adj");
	REQUIRE(part_file.is_open());
	REQUIRE(conn_file.is_open());
	connectivity_from_partition(	part_file, 
									conn_file,
									connectivity, 
									recv_node_ids, 
									send_node_ids );

	REQUIRE(connectivity.size() == 144);
	REQUIRE(connectivity[143].size() == 17);
	REQUIRE(connectivity[143][16].delay == Approx(6.730340));
	REQUIRE(connectivity[143][16].from == 124);
	REQUIRE(connectivity[143][3].from == 152);
	REQUIRE(recv_node_ids.size() == 3);
	REQUIRE(send_node_ids.size() == 3);
	REQUIRE(recv_node_ids[0].first == 0);
	REQUIRE(recv_node_ids[0].second.size() == 3);
	REQUIRE(send_node_ids[0].first == 0);
	REQUIRE(send_node_ids[0].second.size() == 4);
	REQUIRE(recv_node_ids[1].first == 1);
	REQUIRE(recv_node_ids[1].second.size() == 24);
	REQUIRE(send_node_ids[1].first == 1);
	REQUIRE(send_node_ids[1].second.size() == 23);
}
