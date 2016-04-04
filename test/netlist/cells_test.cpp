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

TEST_CASE("cells/assign name","[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	auto u1 = cells_system.create();
	cells.name(u1, "u1");
	REQUIRE(cells.name(u1) == "u1");
}

TEST_CASE("cells/assign cell_type", "[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	ophidian::standard_cell::standard_cells std_cells;
	auto NAND2_X2 = std_cells.cell_create("NAND2_X2");
	auto u1 = cells_system.create();
	cells.standard_cell(u1, NAND2_X2);
	REQUIRE(cells.standard_cell(u1) == NAND2_X2);
}

TEST_CASE("cells/insert pin", "[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);
	auto u1 = cells_system.create();
	auto u1a = pins_system.create();
	cells.insert_pin(u1, u1a);
	REQUIRE(
			std::find(cells.pins(u1).begin(), cells.pins(u1).end(), u1a)
					!= cells.pins(u1).end());
}

TEST_CASE("cells/assign pins", "[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);
	auto u1 = cells_system.create();
	std::vector<ophidian::entity::entity> cell_pins { pins_system.create(),
			pins_system.create(), pins_system.create() };
	cells.pins(u1, cell_pins);
	REQUIRE(cell_pins == cells.pins(u1));
}

TEST_CASE("cells/find interconnections of a cell", "[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);
	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);

	auto u1 = cells_system.create();
	auto u1o = pins_system.create();
	auto n1 = nets_system.create();

	nets.name(n1, "n1");

	nets.connect(n1, u1o);
	pins.net(u1o, n1);
	cells.insert_pin(u1, u1o);
	pins.owner(u1o, u1);

	REQUIRE(nets.name(pins.net(cells.pins(u1).front())) == "n1");
}

TEST_CASE("cells/check cells connected to an interconnection", "[netlist][cells]") {
	ophidian::entity::system cells_system;
	ophidian::netlist::cells cells(cells_system);
	ophidian::entity::system pins_system;
	ophidian::netlist::pins pins(pins_system);
	ophidian::entity::system nets_system;
	ophidian::netlist::nets nets(nets_system);

	auto u1 = cells_system.create();
	auto u1o = pins_system.create();
	cells.insert_pin(u1, u1o);
	pins.owner(u1o, u1);

	auto u2 = cells_system.create();
	auto u2a = pins_system.create();
	cells.insert_pin(u2, u2a);
	pins.owner(u2a, u2);

	auto u3 = cells_system.create();
	auto u4 = cells_system.create();

	auto inp1 = nets_system.create();
	auto n1 = nets_system.create();
	auto n2 = nets_system.create();

	pins.net(u1o, n1); nets.connect(n1, u1o);
	pins.net(u2a, n1); nets.connect(n1, u2a);

	auto interconnection_pins = nets.pins(n1);
	std::vector<ophidian::entity::entity> pins_owners;
	for( auto p : interconnection_pins )
		pins_owners.push_back(pins.owner(p));

	REQUIRE( pins_owners.size() == 2 );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u1) != pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u2) != pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u3) == pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u4) == pins_owners.end() );
}

