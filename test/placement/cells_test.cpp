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

#include <netlist.h>
#include "../placement/cells.h"
#include <standard_cells.h>
#include <boost/geometry.hpp>
#include <library.h>
#include "../geometry/geometry.h"
#include  "../placement/placement.h"

TEST_CASE("placement cell info", "[placement]") {

	// create std cells
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");

	// create dummy lib
	ophidian::placement::library lib(&std_cells);
	std::vector<ophidian::geometry::point<double> > points { { 0.0, 0.0 }, { 0.0,
			200.0 }, { 800.0, 200.0 }, { 800.0, 0.0 }, { 0.0, 0.0 } };

	ophidian::geometry::polygon<ophidian::geometry::point<double> > polygon;
	boost::geometry::append(polygon, points);
	ophidian::geometry::multi_polygon<
	ophidian::geometry::polygon<ophidian::geometry::point<double> > > multipolygon {
		polygon };
	lib.geometry(INV_X1, multipolygon);

	// create netlist
	ophidian::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INV_X1");

	ophidian::placement::cells cells(&netlist);
	cells.position(u1, { 0.0, 0.0 });
	cells.geometry(u1, lib.geometry(INV_X1));
	cells.fixed(u1, false);
	REQUIRE(
			boost::geometry::equals(cells.position(u1),
					ophidian::geometry::point<double>(0.0, 0.0)));
	REQUIRE(boost::geometry::equals(cells.geometry(u1), lib.geometry(INV_X1)));
	REQUIRE(cells.fixed(u1) == false);
}
