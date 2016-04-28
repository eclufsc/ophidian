/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "../catch.hpp"

#include "../timing/library.h"
#include <lemon/list_graph.h>
#include "../timing/graph_arcs_timing.h"
#include "../timing/graph_nodes_timing.h"
#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("sta/timing points timing","[timing][sta]") {

	using namespace ophidian;
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
	using namespace ophidian;
	using namespace boost::units;
	lemon::ListDigraph graph;
	lemon::ListDigraph::Arc arc { graph.addArc(graph.addNode(), graph.addNode()) };

	timing::graph_arcs_timing arcs(graph);
	arcs.delay(arc, quantity<si::time>(10.0 * si::seconds));
	arcs.slew(arc, quantity<si::time>(12.0 * si::seconds));

	REQUIRE(arcs.delay(arc) == quantity<si::time>(10.0 * si::seconds));
	REQUIRE(arcs.slew(arc) == quantity<si::time>(12.0 * si::seconds));

}
