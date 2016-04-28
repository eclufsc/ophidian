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

#include "../placement/lef.h"
#include "../standard_cell/standard_cells.h"
#include "../netlist/netlist.h"
#include "../placement/placement.h"

#include <fstream>

TEST_CASE("lef read", "[lef]") {

    std::ifstream superblue16_lef("benchmarks/superblue16/superblue16.lef", std::ifstream::in);
	REQUIRE( superblue16_lef.good() );
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::netlist::netlist netlist(&std_cells);
	ophidian::placement::library lib(&std_cells);
	ophidian::placement::placement cells(&netlist, &lib);
	ophidian::floorplan::floorplan floorplan;
	ophidian::placement::lef::read(superblue16_lef, &std_cells, &lib, &floorplan);

	auto NOR3_X4 = std_cells.cell_create("NOR3_X4");

	auto pins = std_cells.cell_pins(NOR3_X4);

	REQUIRE( !pins.empty() );

	for(auto p : pins) {
		std::cout << "pin " << std_cells.pin_name(p) << std::endl;
		std::cout << "   offset " << lib.pin_offset(p).x() << ", " << lib.pin_offset(p).y() << std::endl;
	}


}
