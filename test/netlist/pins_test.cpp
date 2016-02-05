#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <pins.h>
#include <nets.h>

TEST_CASE("pins/name","[netlist][pins]") {
	openeda::netlist::pins pins;
	auto u1a = pins.create();
	pins.name(u1a, "u1:a");
	REQUIRE( pins.name(u1a) == "u1:a" );
	pins.name(u1a, "U1:a");
	REQUIRE( pins.name(u1a) == "U1:a" );
}


TEST_CASE("pins/owner","[netlist][pins]") {
	openeda::netlist::cells cells;

	openeda::netlist::pins pins;


	auto u1 = cells.create();
	cells.name(u1, "u1");

	auto u2 = cells.create();
	cells.name(u2, "u2");

	auto u1a = pins.create();
	pins.name(u1a, "u1:a");

	pins.owner(u1a, u1);

	REQUIRE(pins.owner(u1a) == u1);
	REQUIRE( !(pins.owner(u1a) == u2) );

	pins.owner(u1a, u2);

	REQUIRE(pins.owner(u1a) == u2);
	REQUIRE( !(pins.owner(u1a) == u1) );
}


TEST_CASE("pins/net","[netlist][pins]") {

	openeda::netlist::pins pins;

	openeda::netlist::nets nets;


	auto u1a = pins.create();
	auto n1 = nets.create();
	auto n2 = nets.create();

	pins.net(u1a, n1);

	REQUIRE( pins.net(u1a) == n1 );
	REQUIRE( !(pins.net(u1a) == n2) );
}


