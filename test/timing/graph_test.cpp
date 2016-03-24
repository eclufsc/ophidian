#include "../catch.hpp"

#include "../timing/graph.h"
#include "../entity/entity.h"

TEST_CASE("timing graph/", "[timing][graph]")
{

	using namespace openeda;
	timing::graph g;
	REQUIRE( g.nodes_count() == 0 );
	REQUIRE( g.edges_count() == 0 );
}


TEST_CASE("timing graph/add rise node", "[timing][graph]")
{
	using namespace openeda;
	timing::graph g;
	entity::entity pin{0};
	timing::graph::node node = g.rise_node_create(pin);
	REQUIRE( g.rise_node(pin) == node );
	REQUIRE_THROWS( g.fall_node(pin) );
	REQUIRE( g.nodes_count() == 1 );
	REQUIRE( g.pin(node) == pin );
}


TEST_CASE("timing graph/add fall node", "[timing][graph]")
{
	using namespace openeda;
	timing::graph g;
	entity::entity pin{0};
	timing::graph::node node = g.fall_node_create(pin);
	REQUIRE( g.fall_node(pin) == node );
	REQUIRE_THROWS( g.rise_node(pin) );
	REQUIRE( g.nodes_count() == 1 );
}

TEST_CASE("timing graph/add edge", "[timing][graph]")
{
	using namespace openeda;
	timing::graph g;
	entity::entity i{0};
	entity::entity j{1};
	entity::entity tarc{3};
	timing::graph::node node_i = g.fall_node_create(i);
	timing::graph::node node_j = g.rise_node_create(j);

	timing::graph::edge edge = g.edge_create(node_i, node_j, tarc);
	REQUIRE( g.edges_count() == 1 );
	REQUIRE( g.edge_timing_arc(edge) == tarc );
	REQUIRE( g.edge_source(edge) == node_i );
	REQUIRE( g.edge_target(edge) == node_j );
}


TEST_CASE("timing graph/out edges", "[timing][graph]")
{
	using namespace openeda;
	timing::graph g;
	entity::entity i{0};
	entity::entity j{1};
	entity::entity k{2};
	entity::entity tarc{3};
	timing::graph::node node_i = g.fall_node_create(i);
	timing::graph::node node_j = g.rise_node_create(j);
	timing::graph::node node_k = g.rise_node_create(k);

	timing::graph::edge edge1 = g.edge_create(node_i, node_j, tarc);
	timing::graph::edge edge2 = g.edge_create(node_i, node_k, tarc);

	REQUIRE( g.edges_count() == 2 );

	lemon::ListDigraph::ArcMap< bool > visited(g.G());
	lemon::ListDigraph::ArcMap< bool > visited_GOLDEN(g.G());
	visited_GOLDEN[edge1] = true;
	visited_GOLDEN[edge2] = true;

	for(lemon::ListDigraph::OutArcIt it(g.G(), node_i); it != lemon::INVALID; ++it)
		visited[it] = true;
	REQUIRE( visited[edge1] == visited_GOLDEN[edge1] );
	REQUIRE( visited[edge2] == visited_GOLDEN[edge2] );
}
