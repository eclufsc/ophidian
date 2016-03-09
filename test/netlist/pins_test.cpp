#include "../catch.hpp"

#include <entity.h>
#include "../netlist/cells.h"
#include "../netlist/nets.h"
#include "../netlist/pins.h"


TEST_CASE("pins/owner","[netlist][pins]") {
	openeda::entity::system cells_system;
	openeda::netlist::cells cells(cells_system);

	openeda::entity::system pins_system;
	openeda::netlist::pins pins(pins_system);

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
	openeda::entity::system pins_system;
	openeda::netlist::pins pins(pins_system);

	openeda::entity::system nets_system;
	openeda::netlist::nets nets(nets_system);

	auto u1a = pins_system.create();
	auto n1 = nets_system.create();
	auto n2 = nets_system.create();

	pins.net(u1a, n1);

	REQUIRE(pins.net(u1a) == n1);
	REQUIRE(!(pins.net(u1a) == n2));
}

TEST_CASE("pins/std_cell pin","[netlist][pins]") {

	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1_a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1_o = std_cells.pin_create(INV_X1, "o");

	openeda::entity::system pins_system;
	openeda::netlist::pins pins(pins_system);

	auto pin = pins_system.create();
	pins.standard_cell_pin(pin, INV_X1_a);
	auto pin2 = pins_system.create();
	pins.standard_cell_pin(pin2, INV_X1_o);

	REQUIRE(pins.standard_cell_pin(pin) == INV_X1_a);
	REQUIRE(pins.standard_cell_pin(pin2) == INV_X1_o);

}

