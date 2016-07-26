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

#include "../placement/placement.h"

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include  <iostream>

TEST_CASE("placement/place pads", "[placement]") {

	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	auto inp1 = netlist.PI_insert("inp1");
	auto out = netlist.PO_insert("out");

	ophidian::placement::library lib { &std_cells };
	ophidian::placement::placement placement { &netlist, &lib };
	ophidian::geometry::point<double> position { 1.0, 2.0 };
	ophidian::geometry::point<double> position2 { 3.0, 4.0 };
	placement.pad_position(inp1, position);
	placement.pad_position(out, position2);

	REQUIRE(placement.pin_position(inp1).x() == position.x());
	REQUIRE(placement.pin_position(inp1).y() == position.y());
	REQUIRE(placement.pin_position(out).x() == position2.x());
	REQUIRE(placement.pin_position(out).y() == position2.y());

}

TEST_CASE("placement/place cell", "[placement]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib { &std_cells };
	ophidian::placement::placement placement { &netlist, &lib };

	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");

	lib.pin_offset(INV_X1a, { 1.0, 2.0 });
	lib.pin_offset(INV_X1o, { 3.0, 4.0 });

	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u1a = netlist.pin_insert(u1, "a");
	auto u1o = netlist.pin_insert(u1, "o");

	placement.cell_position(u1, { 100.0, 200.0 });

	REQUIRE(placement.cell_position(u1).x() == 100.0);
	REQUIRE(placement.cell_position(u1).y() == 200.0);

	REQUIRE(placement.pin_position(u1a).x() == 101.0);
	REQUIRE(placement.pin_position(u1a).y() == 202.0);

	REQUIRE(placement.pin_position(u1o).x() == 103.0);
	REQUIRE(placement.pin_position(u1o).y() == 204.0);

}

TEST_CASE("placement/trying to move fixed cell", "[placement]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib { &std_cells };
	ophidian::placement::placement placement { &netlist, &lib };

	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");

	lib.pin_offset(INV_X1a, { 1.0, 2.0 });
	lib.pin_offset(INV_X1o, { 3.0, 4.0 });

	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u1a = netlist.pin_insert(u1, "a");
	auto u1o = netlist.pin_insert(u1, "o");

	placement.cell_position(u1, { 100.0, 200.0 });

	REQUIRE(placement.cell_position(u1).x() == 100.0);
	REQUIRE(placement.cell_position(u1).y() == 200.0);

	REQUIRE(placement.pin_position(u1a).x() == 101.0);
	REQUIRE(placement.pin_position(u1a).y() == 202.0);

	REQUIRE(placement.pin_position(u1o).x() == 103.0);
	REQUIRE(placement.pin_position(u1o).y() == 204.0);

	placement.cell_fixed(u1, true);
	placement.cell_position(u1, {300.0, 400.0});

    REQUIRE(placement.cell_position(u1).x() == 100.0);
    REQUIRE(placement.cell_position(u1).y() == 200.0);

    REQUIRE(placement.pin_position(u1a).x() == 101.0);
    REQUIRE(placement.pin_position(u1a).y() == 202.0);

    REQUIRE(placement.pin_position(u1o).x() == 103.0);
    REQUIRE(placement.pin_position(u1o).y() == 204.0);
}
