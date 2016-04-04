/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../catch.hpp"

#include "../timing/graph.h"
#include "../entity/entity.h"

TEST_CASE("timing graph/", "[timing][graph]") {

	using namespace openeda;
	timing::graph g;
	REQUIRE(g.nodes_count() == 0);
	REQUIRE(g.edges_count() == 0);
}

TEST_CASE("timing graph/add rise node", "[timing][graph]") {
	using namespace openeda;
	timing::graph g;
	entity::entity pin { 0 };
	timing::graph::node node = g.rise_node_create(pin);
	REQUIRE(g.rise_node(pin) == node);
	REQUIRE_THROWS(g.fall_node(pin));
	REQUIRE(g.nodes_count() == 1);
	REQUIRE(g.pin(node) == pin);
}

TEST_CASE("timing graph/add fall node", "[timing][graph]") {
	using namespace openeda;
	timing::graph g;
	entity::entity pin { 0 };
	timing::graph::node node = g.fall_node_create(pin);
	REQUIRE(g.fall_node(pin) == node);
	REQUIRE_THROWS(g.rise_node(pin));
	REQUIRE(g.nodes_count() == 1);
}

TEST_CASE("timing graph/add edge", "[timing][graph]") {
	using namespace openeda;
	timing::graph g;
	entity::entity i { 0 };
	entity::entity j { 1 };
	entity::entity tarc { 3 };
	timing::graph::node node_i = g.fall_node_create(i);
	timing::graph::node node_j = g.rise_node_create(j);

	timing::graph::edge edge = g.edge_create(node_i, node_j, timing::edge_types::TIMING_ARC, tarc);
	REQUIRE(g.edges_count() == 1);
	REQUIRE(g.edge_type(edge) == timing::edge_types::TIMING_ARC);
	REQUIRE(g.edge_entity(edge) == tarc);
	REQUIRE(g.edge_source(edge) == node_i);
	REQUIRE(g.edge_target(edge) == node_j);
}

TEST_CASE("timing graph/out edges", "[timing][graph]") {
	using namespace openeda;
	timing::graph g;
	entity::entity i { 0 };
	entity::entity j { 1 };
	entity::entity k { 2 };
	entity::entity tarc { 3 };
	timing::graph::node node_i = g.fall_node_create(i);
	timing::graph::node node_j = g.rise_node_create(j);
	timing::graph::node node_k = g.rise_node_create(k);

	timing::graph::edge edge1 = g.edge_create(node_i, node_j, timing::edge_types::TIMING_ARC, tarc);
	timing::graph::edge edge2 = g.edge_create(node_i, node_k, timing::edge_types::TIMING_ARC, tarc);

	REQUIRE(g.edges_count() == 2);

	lemon::ListDigraph::ArcMap<bool> visited(g.G());
	lemon::ListDigraph::ArcMap<bool> visited_GOLDEN(g.G());
	visited_GOLDEN[edge1] = true;
	visited_GOLDEN[edge2] = true;

	for (lemon::ListDigraph::OutArcIt it(g.G(), node_i); it != lemon::INVALID; ++it)
		visited[it] = true;
	REQUIRE(visited[edge1] == visited_GOLDEN[edge1]);
	REQUIRE(visited[edge2] == visited_GOLDEN[edge2]);
}

#include "../netlist/netlist.h"
#include "../timing/graph_builder.h"

namespace utils {
template<class T>
struct transitions {
	T rise;
	T fall;
};
}

TEST_CASE("graph/construct from netlist", "[timing][graph]") {
	using namespace openeda;

	standard_cell::standard_cells std_cells;
	netlist::netlist netlist { &std_cells };

	netlist.PI_insert("inp");
	netlist.PO_insert("out");
	netlist.connect(netlist.net_insert("inp"), netlist.PI_insert("inp"));
	netlist.connect(netlist.net_insert("out"), netlist.PO_insert("out"));
	netlist.connect(netlist.net_insert("inp"), netlist.pin_insert(netlist.cell_insert("u1", "INV_X1"), "a"));
	netlist.connect(netlist.net_insert("out"), netlist.pin_insert(netlist.cell_insert("u1", "INV_X1"), "o"));

	std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name("inp")), standard_cell::pin_directions::OUTPUT);
	std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name("out")), standard_cell::pin_directions::INPUT);
	std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name("u1:a")), standard_cell::pin_directions::INPUT);
	std_cells.pin_direction(netlist.pin_std_cell(netlist.pin_by_name("u1:o")), standard_cell::pin_directions::OUTPUT);

	/*
	 * inp -> a (u1:INV_X1) o -> out
	 *
	 */

	timing::library_timing_arcs tarcs { &std_cells };
	timing::library timing_lib { &tarcs, &std_cells };

	entity::entity tarc { timing_lib.timing_arc_create(netlist.pin_std_cell(netlist.pin_by_name("u1:a")), netlist.pin_std_cell(netlist.pin_by_name("u1:o"))) };

	timing::graph graph;
	timing::design_constraints dc;

	timing::graph_builder::build(netlist, timing_lib, dc, graph);

	timing::graph graph_GOLDEN;

	utils::transitions<timing::graph::node> inp { graph_GOLDEN.rise_node_create(netlist.pin_by_name("inp")), graph_GOLDEN.fall_node_create(netlist.pin_by_name("inp")) };
	utils::transitions<timing::graph::node> u1a { graph_GOLDEN.rise_node_create(netlist.pin_by_name("u1:a")), graph_GOLDEN.fall_node_create(netlist.pin_by_name("u1:a")) };
	utils::transitions<timing::graph::node> u1o { graph_GOLDEN.rise_node_create(netlist.pin_by_name("u1:o")), graph_GOLDEN.fall_node_create(netlist.pin_by_name("u1:o")) };
	utils::transitions<timing::graph::node> out { graph_GOLDEN.rise_node_create(netlist.pin_by_name("out")), graph_GOLDEN.fall_node_create(netlist.pin_by_name("out")) };

	graph_GOLDEN.edge_create(inp.rise, u1a.rise, timing::edge_types::NET, netlist.net_by_name("inp"));
	graph_GOLDEN.edge_create(inp.fall, u1a.fall, timing::edge_types::NET, netlist.net_by_name("inp"));
	graph_GOLDEN.edge_create(u1a.rise, u1o.fall, timing::edge_types::TIMING_ARC, tarc);
	graph_GOLDEN.edge_create(u1a.fall, u1o.rise, timing::edge_types::TIMING_ARC, tarc);
	graph_GOLDEN.edge_create(u1o.rise, out.rise, timing::edge_types::NET, netlist.net_by_name("out"));
	graph_GOLDEN.edge_create(u1o.fall, out.fall, timing::edge_types::NET, netlist.net_by_name("out"));

	REQUIRE((graph.edges_count() == graph_GOLDEN.edges_count()));
	REQUIRE((graph.nodes_count() == graph_GOLDEN.nodes_count()));
}
