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

#include <def.h>
#include "../placement/placement.h"
#include <boost/geometry/algorithms/equals.hpp>
#include <iostream>
#include <fstream>

TEST_CASE("def read", "[def]") {

    std::ifstream superblue16_def("input_files/superblue16.def", std::ifstream::in);
	REQUIRE( superblue16_def.good() );
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib(&std_cells);
	ophidian::placement::placement cells(&netlist, &lib);
	ophidian::floorplan::floorplan floorplan;
	ophidian::placement::def::read(superblue16_def, &netlist, &cells, &floorplan);

	auto position = cells.cell_position(netlist.cell_insert("o641083", "INV_Z4"));
	auto golden = ophidian::geometry::point<double> { 7214680.0, 1402200.0 };

	auto pad_position = cells.pin_position(netlist.pin_by_name("iccad_clk"));
	auto golden_pad_position = ophidian::geometry::point<double> {5807950, 6121800};
	golden_pad_position.x(golden_pad_position.x() + 0.5*(-140+140));
	golden_pad_position.y(golden_pad_position.y() + 0.5*(0+280));

	REQUIRE( boost::geometry::equals( position, golden ) );
	REQUIRE( boost::geometry::equals( pad_position, golden_pad_position ) );

}
