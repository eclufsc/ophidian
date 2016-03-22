#include "../catch.hpp"

#include "../timing/library.h"
#include <lemon/list_graph.h>
#include "../timing/graph_arcs_timing.h"
#include "../timing/graph_nodes_timing.h"
#include "../timing/sta_timing_point_calculator.h"
#include "../timing/sta_timing_arc_calculator.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("sta/timing points timing","[timing][sta]") {

	using namespace openeda;
	using namespace boost::units;
	lemon::ListDigraph graph;
	lemon::ListDigraph::Node node { graph.addNode() };

	timing::graph_nodes_timing points(graph);
	points.arrival(node, quantity<si::time>(1.0 * si::second));
	points.slew(node, quantity<si::time>(3.0 * si::second));
	points.required(node, quantity<si::time>(5.0 * si::second));
	points.load(node, quantity<si::capacitance>(6.0 * si::farads));

	REQUIRE(points.arrival(node) == quantity<si::time>(1.0 * si::second));
	REQUIRE(points.slew(node) == quantity<si::time>(3.0 * si::second));
	REQUIRE(points.required(node) == quantity<si::time>(5.0 * si::second));
	REQUIRE(points.load(node) == quantity<si::capacitance>(6.0 * si::farads));

}
TEST_CASE("sta/timing arcs timing", "[timing][sta]") {
	using namespace openeda;
	using namespace boost::units;
	lemon::ListDigraph graph;
	lemon::ListDigraph::Arc arc { graph.addArc(graph.addNode(), graph.addNode()) };

	timing::graph_arcs_timing arcs(graph);
	arcs.delay(arc, quantity<si::time>(10.0 * si::seconds));
	arcs.slew(arc, quantity<si::time>(12.0 * si::seconds));

	REQUIRE(arcs.delay(arc) == quantity<si::time>(10.0 * si::seconds));
	REQUIRE(arcs.slew(arc) == quantity<si::time>(12.0 * si::seconds));
}

TEST_CASE("sta/propagate maximum input arrival time", "[timing][sta]") {
	using namespace openeda;
	using namespace boost::units;
	lemon::ListDigraph graph;

	/*
	 * i->k
	 * j->k
	 *
	 * */

	using Node = lemon::ListDigraph::Node;
	using Arc = lemon::ListDigraph::Arc;

	Node i = graph.addNode();
	Node j = graph.addNode();
	Node k = graph.addNode();

	Arc ik = graph.addArc(i, k);
	Arc jk = graph.addArc(j, k);

	timing::graph_nodes_timing points(graph);
	timing::graph_arcs_timing arcs(graph);

	points.arrival(i, quantity<si::time>(1.0 * si::pico * si::second));
	points.arrival(j, quantity<si::time>(2.0 * si::pico * si::second));

	arcs.delay(ik, quantity<si::time>(3.0 * si::pico * si::second));
	arcs.delay(jk, quantity<si::time>(1.0 * si::pico * si::second));

	timing::sta_timing_point_calculator calculator;
	calculator.update(graph, arcs, points, k);

	REQUIRE(points.arrival(k) == quantity<si::time>(4.0 * si::pico * si::second));
}

#include "../timing/liberty.h"
TEST_CASE("sta/calculate timing arc delay", "[timing][sta]") {
	using namespace openeda;
	using namespace boost::units;
	lemon::ListDigraph graph;

	using Node = lemon::ListDigraph::Node;
	using Arc = lemon::ListDigraph::Arc;

	Node i = graph.addNode();
	Node k = graph.addNode();

	Arc ik = graph.addArc(i, k);
	std::vector<Arc> graph_arcs { ik };

	timing::graph_nodes_timing points(graph);
	timing::graph_arcs_timing arcs_rise(graph);
	timing::graph_arcs_timing arcs_fall(graph);

	points.slew(i, quantity<si::time>(80.0 * si::pico * si::seconds));
	points.load(k, quantity<si::capacitance>(1.00 * si::femto * si::farads));

	standard_cell::standard_cells std_cell;
	timing::library_timing_arcs tarcs { &std_cell };
	timing::library library(&tarcs, &std_cell);
	timing::liberty::read("benchmarks/superblue16/superblue16_Late.lib", library);
	timing::sta_timing_arc_calculator calculator(library, graph, points, arcs_rise, arcs_fall);

	auto INV_X1 = std_cell.cell_create("INV_X1");
	auto INV_X1a = std_cell.pin_create(INV_X1, "a");
	auto INV_X1o = std_cell.pin_create(INV_X1, "o");

	calculator.timing_arc(ik, library.timing_arc(INV_X1a, INV_X1o));
	calculator.update(graph_arcs.begin(), graph_arcs.end());

	REQUIRE( arcs_rise.delay(ik) == quantity<si::time>(41.94*si::pico*si::seconds) );

}

