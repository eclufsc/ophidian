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

#include <entity.h>
#include "../netlist/cells.h"
#include "../netlist/nets.h"
#include "../netlist/pins.h"

TEST_CASE("nets/name","[netlist][nets]") {

	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);

	auto n1 = nets_system.create();
	nets.name(n1, "n1");

	REQUIRE(nets.name(n1) == "n1");

}

TEST_CASE("nets/assign pins","[netlist][nets]") {

	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);

	ophidian::entity::system pin_sys;

	auto n1 = nets_system.create();

	std::vector<ophidian::entity::entity> net_pins { pin_sys.create(),
			pin_sys.create(), pin_sys.create() };

	nets.pins(n1, net_pins);

	REQUIRE(nets.pins(n1) == net_pins);

}

TEST_CASE("nets/connect pin","[netlist][nets]") {

	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);
	ophidian::entity::system pin_sys;

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

	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);
	ophidian::entity::system pin_sys;

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

