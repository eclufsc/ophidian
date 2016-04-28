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

#include <entity.h>
#include "../netlist/cells.h"
#include "../netlist/nets.h"
#include "../netlist/pins.h"


TEST_CASE("pins/owner","[netlist][pins]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);

	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);

	auto u1 = cells_system.create();
	cells.name(u1, "u1");

	auto u2 = cells_system.create();
	cells.name(u2, "u2");

	auto u1a = pins_system.create();

	pins.owner(u1a, u1);

	REQUIRE(pins.owner(u1a) == u1);
	REQUIRE(!(pins.owner(u1a) == u2));

	pins.owner(u1a, u2);

	REQUIRE(pins.owner(u1a) == u2);
	REQUIRE(!(pins.owner(u1a) == u1));
}

TEST_CASE("pins/net","[netlist][pins]") {
	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);

	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);

	auto u1a = pins_system.create();
	auto n1 = nets_system.create();
	auto n2 = nets_system.create();

	pins.net(u1a, n1);

	REQUIRE(pins.net(u1a) == n1);
	REQUIRE(!(pins.net(u1a) == n2));
}

TEST_CASE("pins/std_cell pin","[netlist][pins]") {

	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1_a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1_o = std_cells.pin_create(INV_X1, "o");

	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);

	auto pin = pins_system.create();
	pins.standard_cell_pin(pin, INV_X1_a);
	auto pin2 = pins_system.create();
	pins.standard_cell_pin(pin2, INV_X1_o);

	REQUIRE(pins.standard_cell_pin(pin) == INV_X1_a);
	REQUIRE(pins.standard_cell_pin(pin2) == INV_X1_o);

}

