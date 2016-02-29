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
	unsigned long n_local_nodes = connectivity_from_partition(	
			part_file, 
			conn_file,
			connectivity, 
			recv_node_ids, 
			send_node_ids );

	REQUIRE(n_local_nodes == 144);
	REQUIRE(connectivity.size() == 193); //local + recv nodes
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
	REQUIRE(recv_node_ids[2].first == 2);
	REQUIRE(recv_node_ids[2].second.size() == 22);
}

TEST_CASE("Reading regional connectivity"){
	global_connectivity_type connectivity;
	std::vector< std::vector< std::size_t > > region_nodes;
	std::vector< std::size_t >nodes_region;

	std::ifstream reg_file("data/test_init.4.reg");
	std::ifstream conn_file("data/test_region_4.mtx");
	REQUIRE(reg_file.is_open());
	REQUIRE(conn_file.is_open());

	read_regional_mapping(	reg_file, 
							//conn_file, 
							//connectivity,
							region_nodes, 
							nodes_region);
	connectivity = connectivity_from_mtx(conn_file);
	REQUIRE(connectivity.size() == 4);
	REQUIRE(connectivity[0].size() == 1);
	REQUIRE(connectivity[1].size() == 1);
	REQUIRE(connectivity[2].size() == 1);
	REQUIRE(connectivity[3].size() == 1);
	REQUIRE(region_nodes.size() == 4);
	REQUIRE(nodes_region.size() == 20);

	static const std::size_t arr[] = {0, 0, 0, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 0, 0};
	std::vector< std::size_t > expected_regions(arr, arr + sizeof(arr) / sizeof(arr[0]) );
	REQUIRE(expected_regions == nodes_region);

	static const std::size_t arr1[] = {3,4,5,6,7};
	std::vector< std::size_t > expected_nodes(arr1, arr1 + sizeof(arr1) / sizeof(arr1[0]) );
	REQUIRE(expected_nodes == region_nodes[2]);
}
