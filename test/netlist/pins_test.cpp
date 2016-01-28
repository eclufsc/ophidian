#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <pins.h>
#include <nets.h>

TEST_CASE("pins/name","[netlist][pins]") {
	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);
	auto u1a = pin_system.create();
	pins.name(u1a, "u1:a");
	REQUIRE( pins.name(u1a) == "u1:a" );
	pins.name(u1a, "U1:a");
	REQUIRE( pins.name(u1a) == "U1:a" );
}


TEST_CASE("pins/owner","[netlist][pins]") {
	openeda::entity::system cell_system;
	openeda::netlist::cells cells(&cell_system);

	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);


	auto u1 = cell_system.create();
	cells.name(u1, "u1");

	auto u2 = cell_system.create();
	cells.name(u2, "u2");

	auto u1a = pin_system.create();
	pins.name(u1a, "u1:a");

	pins.owner(u1a, u1);

	REQUIRE(pins.owner(u1a) == u1);
	REQUIRE( !(pins.owner(u1a) == u2) );

	pins.owner(u1a, u2);

	REQUIRE(pins.owner(u1a) == u2);
	REQUIRE( !(pins.owner(u1a) == u1) );
}


TEST_CASE("pins/net","[netlist][pins]") {

	openeda::entity::system pin_system;
	openeda::netlist::pins pins(&pin_system);

	openeda::entity::system net_system;
	openeda::netlist::nets nets(&net_system);


	auto u1a = pin_system.create();
	auto n1 = net_system.create();
	auto n2 = net_system.create();

	pins.net(u1a, n1);

	REQUIRE( pins.net(u1a) == n1 );
	REQUIRE( !(pins.net(u1a) == n2) );
}


