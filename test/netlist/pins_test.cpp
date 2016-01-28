#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <pins.h>

TEST_CASE("pins/assign name to pin","[netlist][pins]") {
	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);
	auto u1a = pin_system.create();
	pins.name(u1a, "u1:a");
	REQUIRE( pins.name(u1a) == "u1:a" );
}


TEST_CASE("pins/assign owner to pin","[netlist][pins]") {
	openeda::entity::system cell_system;
	openeda::netlist::cells cells(&cell_system);

	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);

	openeda::standard_cell::standard_cells std_cells;
	auto NAND2_X2 = std_cells.create("NAND2_X2");

	auto u1 = cell_system.create();
	cells.name(u1, "u1");
	cells.standard_cell(u1, NAND2_X2);

	auto u2 = cell_system.create();
	cells.name(u2, "u2");
	cells.standard_cell(u1, NAND2_X2);

	auto u1a = pin_system.create();
	pins.name(u1a, "u1:a");

	pins.owner(u1a, u1);

	REQUIRE(pins.owner(u1a) == u1);
	REQUIRE( !(pins.owner(u1a) == u2) );

	pins.owner(u1a, u2);

	REQUIRE(pins.owner(u1a) == u2);
	REQUIRE( !(pins.owner(u1a) == u1) );
}
