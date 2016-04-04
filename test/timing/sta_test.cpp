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

#include "../timing/library.h"
#include <lemon/list_graph.h>
#include "../timing/graph_arcs_timing.h"
#include "../timing/graph_nodes_timing.h"
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
