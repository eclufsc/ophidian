#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <pins.h>
#include <nets.h>

TEST_CASE("nets/name","[netlist][nets]") {

	openeda::entity::system sys;
	openeda::netlist::nets nets(&sys);

	auto n1 = sys.create();
	nets.name(n1, "n1");

	REQUIRE(nets.name(n1) == "n1");

}

TEST_CASE("nets/assign pins","[netlist][nets]") {

	openeda::entity::system sys;
	openeda::netlist::nets nets(&sys);

	openeda::entity::system pin_sys;

	auto n1 = sys.create();

	std::vector<openeda::entity::entity> net_pins { pin_sys.create(),
			pin_sys.create(), pin_sys.create() };

	nets.pins(n1, net_pins);

	REQUIRE(nets.pins(n1) == net_pins);

}

TEST_CASE("nets/connect pin","[netlist][nets]") {

	openeda::entity::system sys;
	openeda::netlist::nets nets(&sys);
	openeda::entity::system pin_sys;

	auto n1 = sys.create();

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

	openeda::entity::system sys;
	openeda::netlist::nets nets(&sys);
	openeda::entity::system pin_sys;

	auto n1 = sys.create();

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

