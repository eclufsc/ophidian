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

TEST_CASE("nets/name","[netlist][nets]") {

    ophidian::entity_system::entity_system nets_system;
	ophidian::netlist::nets nets(nets_system);

	auto n1 = nets_system.create();
	nets.name(n1, "n1");

	REQUIRE(nets.name(n1) == "n1");

}

TEST_CASE("nets/assign pins","[netlist][nets]") {

    ophidian::entity_system::entity_system nets_system;
	ophidian::netlist::nets nets(nets_system);

    ophidian::entity_system::entity_system pin_sys;

	auto n1 = nets_system.create();

    std::vector<ophidian::entity_system::entity> net_pins { pin_sys.create(),
			pin_sys.create(), pin_sys.create() };

	nets.pins(n1, net_pins);

	REQUIRE(nets.pins(n1) == net_pins);

}

TEST_CASE("nets/connect pin","[netlist][nets]") {

    ophidian::entity_system::entity_system nets_system;
	ophidian::netlist::nets nets(nets_system);
    ophidian::entity_system::entity_system pin_sys;

	auto n1 = nets_system.create();

	auto connected = pin_sys.create();
	auto unconnected = pin_sys.create();

	nets.connect(n1, connected);

	auto net_pins = nets.pins(n1);

	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), connected)
					!= net_pins.end());
	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), unconnected)
					== net_pins.end());

}

TEST_CASE("nets/disconnect pin","[netlist][nets]") {

    ophidian::entity_system::entity_system nets_system;
	ophidian::netlist::nets nets(nets_system);
    ophidian::entity_system::entity_system pin_sys;

	auto n1 = nets_system.create();

	auto connected = pin_sys.create();
	auto unconnected = pin_sys.create();

	nets.connect(n1, connected);
	nets.connect(n1, unconnected);

	auto net_pins = nets.pins(n1);

	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), connected)
					!= net_pins.end());
	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), unconnected)
					!= net_pins.end());

	REQUIRE_NOTHROW( nets.disconnect(n1, unconnected) );

	net_pins = nets.pins(n1);

	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), connected)
					!= net_pins.end());
	REQUIRE(
			std::find(net_pins.begin(), net_pins.end(), unconnected)
					== net_pins.end());


	REQUIRE_THROWS( nets.disconnect(n1, unconnected) );

}

