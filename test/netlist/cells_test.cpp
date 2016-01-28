#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <nets.h>

TEST_CASE("cells/assign name","[netlist][cells]") {
	openeda::entity::system system;
	openeda::netlist::cells cells(&system);
	auto u1 = system.create();
	cells.name(u1, "u1");
	REQUIRE(cells.name(u1) == "u1");
}

TEST_CASE("cells/assign cell_type", "[netlist][cells]") {
	openeda::entity::system system;
	openeda::netlist::cells cells(&system);
	openeda::standard_cell::standard_cells std_cells;
	auto NAND2_X2 = std_cells.create("NAND2_X2");
	auto u1 = system.create();
	cells.standard_cell(u1, NAND2_X2);
	REQUIRE(cells.standard_cell(u1) == NAND2_X2);
}

TEST_CASE("cells/insert pin", "[netlist][cells]") {
	openeda::entity::system cell_system;
	openeda::netlist::cells cells(&cell_system);
	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);
	auto u1 = cell_system.create();
	auto u1a = pin_system.create();
	cells.insert_pin(u1, u1a);
	REQUIRE(
			std::find(cells.pins(u1).begin(), cells.pins(u1).end(), u1a)
					!= cells.pins(u1).end());
}

TEST_CASE("cells/assign pins", "[netlist][cells]") {
	openeda::entity::system cell_system;
	openeda::netlist::cells cells(&cell_system);
	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);
	auto u1 = cell_system.create();
	std::vector<openeda::entity::entity> cell_pins { pin_system.create(),
			pin_system.create(), pin_system.create() };
	cells.pins(u1, cell_pins);
	REQUIRE(cell_pins == cells.pins(u1));
}

TEST_CASE("cells/find interconnections of a cell", "[netlist][cells]") {
	openeda::entity::system cell_system, pin_system, net_system;
	openeda::netlist::cells cells(&cell_system);
	openeda::netlist::pins pins(&pin_system);
	openeda::netlist::nets nets(&net_system);

	auto u1 = cell_system.create();
	auto u1o = pin_system.create();
	auto n1 = net_system.create();

	nets.name(n1, "n1");

	nets.connect(n1, u1o);
	pins.net(u1o, n1);
	cells.insert_pin(u1, u1o);
	pins.owner(u1o, u1);

	REQUIRE(nets.name(pins.net(cells.pins(u1).front())) == "n1");
}

TEST_CASE("cells/check cells connected to an interconnection", "[netlist][cells]") {
	openeda::entity::system cell_system, pin_system, net_system;
	openeda::netlist::cells cells(&cell_system);
	openeda::netlist::pins pins(&pin_system);
	openeda::netlist::nets nets(&net_system);

	auto u1 = cell_system.create();
	auto u1o = pin_system.create();
	cells.insert_pin(u1, u1o);
	pins.owner(u1o, u1);

	auto u2 = cell_system.create();
	auto u2a = pin_system.create();
	cells.insert_pin(u2, u2a);
	pins.owner(u2a, u2);

	auto u3 = cell_system.create();
	auto u4 = cell_system.create();

	auto inp1 = net_system.create();
	auto n1 = net_system.create();
	auto n2 = net_system.create();

	pins.net(u1o, n1); nets.connect(n1, u1o);
	pins.net(u2a, n1); nets.connect(n1, u2a);

	auto interconnection_pins = nets.pins(n1);
	std::vector<openeda::entity::entity> pins_owners;
	for( auto p : interconnection_pins )
		pins_owners.push_back(pins.owner(p));

	REQUIRE( pins_owners.size() == 2 );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u1) != pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u2) != pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u3) == pins_owners.end() );
	REQUIRE( std::find(pins_owners.begin(), pins_owners.end(), u4) == pins_owners.end() );
}

