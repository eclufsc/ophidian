#include "../catch.hpp"

#include <entity.h>
#include <cells.h>
#include <nets.h>
#include <netlist.h>

TEST_CASE("netlist/ empty","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist empty(&std_cells);
	REQUIRE(empty.cell_count() == 0);
	REQUIRE(empty.net_count() == 0);
	REQUIRE(empty.pin_count() == 0);
}

TEST_CASE("netlist/insert cell","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	REQUIRE(netlist.cell_count() == 1);
	REQUIRE(std_cells.size() == 1);
	REQUIRE(netlist.cell_name(u1) == "u1");
	auto u2 = netlist.cell_insert("u2", "INVX1");
	REQUIRE(netlist.cell_count() == 2);
	REQUIRE(std_cells.size() == 1);
	REQUIRE(netlist.cell_name(u2) == "u2");
	REQUIRE(netlist.cell_name(u1) == "u1");
}

TEST_CASE("netlist/cant insert cell twice","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u12 = netlist.cell_insert("u1", "INVX1");
	REQUIRE(u1 == u12);
	REQUIRE(netlist.cell_count() == 1);
}

TEST_CASE("netlist/remove cell","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u2 = netlist.cell_insert("u2", "INVX1");
	REQUIRE_NOTHROW(netlist.cell_remove(u1));
	REQUIRE_THROWS(netlist.cell_remove(u1));
	REQUIRE(netlist.cell_count() == 1);
	REQUIRE_THROWS(netlist.cell_name(u1));
}

TEST_CASE("netlist/insert cell after removing","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u2 = netlist.cell_insert("u2", "INVX1");
	netlist.cell_remove(u1);
	u1 = netlist.cell_insert("u1", "INVX1");
	REQUIRE(netlist.cell_count() == 2);
}

TEST_CASE("netlist/insert pin to cell","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u1a = netlist.pin_insert(u1, "u1:a");
	auto u1o = netlist.pin_insert(u1, "u1:o");
	REQUIRE(netlist.pin_count() == 2);
	auto cell_pins = netlist.cell_pins(u1);
	REQUIRE(cell_pins.size() == 2);
	REQUIRE(
			std::find(cell_pins.begin(), cell_pins.end(), u1a)
					!= cell_pins.end());
	REQUIRE(
			std::find(cell_pins.begin(), cell_pins.end(), u1o)
					!= cell_pins.end());
	REQUIRE(netlist.pin_owner(u1a) == u1);
	REQUIRE(netlist.pin_owner(u1o) == u1);
}

TEST_CASE("netlist/remove cell and owned pins","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u1a = netlist.pin_insert(u1, "u1:a");
	auto u1o = netlist.pin_insert(u1, "u1:o");

	auto u2 = netlist.cell_insert("u2", "INVX1");
	auto u2a = netlist.pin_insert(u2, "u2:a");
	auto u2o = netlist.pin_insert(u2, "u2:o");

	netlist.cell_remove(u1);

	REQUIRE(netlist.pin_count() == 2);
	REQUIRE_THROWS(netlist.pin_owner(u1a));
	REQUIRE_THROWS(netlist.pin_owner(u1o));

	REQUIRE_NOTHROW(netlist.pin_owner(u2a) == u2);
	REQUIRE_NOTHROW(netlist.pin_owner(u2o) == u2);
}

TEST_CASE("netlist/insert net","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto n1 = netlist.net_insert("n1");
	REQUIRE(netlist.net_count() == 1);
	REQUIRE(netlist.net_name(n1) == "n1");
	REQUIRE(netlist.net_pins(n1).empty());
}

TEST_CASE("netlist/ cannot insert net twice","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto n1 = netlist.net_insert("n1");
	auto n12 = netlist.net_insert("n1");
	REQUIRE(netlist.net_count() == 1);
	REQUIRE(netlist.net_name(n1) == "n1");
	REQUIRE(n1 == n12);
}

TEST_CASE("netlist/connect","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u1a = netlist.pin_insert(u1, "u1:a");
	auto n1 = netlist.net_insert("n1");
	auto n2 = netlist.net_insert("n2");
	netlist.connect(n1, u1a);
	auto net_pins = netlist.net_pins(n1);
	REQUIRE_NOTHROW(netlist.connect(n1, u1a)); // can connect twice if is the same net
	REQUIRE_THROWS(netlist.connect(n2, u1a)); // cannot connect twice if is not the same net
	REQUIRE(std::find(net_pins.begin(), net_pins.end(), u1a) != net_pins.end());
	REQUIRE(netlist.pin_net(u1a) == n1);
	REQUIRE(net_pins.size() == 1);
}

TEST_CASE("netlist/disconnect","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u1a = netlist.pin_insert(u1, "u1:a");
	REQUIRE_THROWS(netlist.disconnect(u1a)); // cannot disconnect a disconnected pin
	auto n1 = netlist.net_insert("n1");
	netlist.connect(n1, u1a);
	REQUIRE_NOTHROW(netlist.disconnect(u1a));
	auto net_pins = netlist.net_pins(n1);
	REQUIRE(net_pins.empty());
	REQUIRE(!(netlist.pin_net(u1) == n1));
	REQUIRE_THROWS(netlist.disconnect(u1a)); // cannot disconnect twice
}

TEST_CASE("netlist/remove cell and disconnect all pins before removing them","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto inp1 = netlist.net_insert("inp1");
	auto u1a = netlist.pin_insert(u1, "u1:a");
	netlist.connect(inp1, u1a);
	auto u1o = netlist.pin_insert(u1, "u1:o");
	auto n1 = netlist.net_insert("n1");
	netlist.connect(n1, u1o);

	auto u2 = netlist.cell_insert("u2", "INVX1");
	auto u2a = netlist.pin_insert(u2, "u2:a");
	netlist.connect(n1, u2a);
	auto u2o = netlist.pin_insert(u2, "u2:o");
	auto n2 = netlist.net_insert("n2");
	netlist.connect(n2, u2o);
	netlist.cell_remove(u1);

	auto inp1_pins = netlist.net_pins(inp1);
	auto n1_pins = netlist.net_pins(n1);

	REQUIRE(
			std::find(inp1_pins.begin(), inp1_pins.end(), u1a)
					== inp1_pins.end());
	REQUIRE(std::find(n1_pins.begin(), n1_pins.end(), u1o) == n1_pins.end());

	REQUIRE(n1_pins.size() == 1); // u2:a
	REQUIRE(inp1_pins.empty());

}

TEST_CASE("netlist/remove net", "[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto u1 = netlist.cell_insert("u1", "INVX1");
	auto u1a  = netlist.pin_insert(u1, "u1:a");
	auto n1 = netlist.net_insert("u1");
	auto pi = netlist.PI_insert("pi");

	netlist.connect(n1, pi);
	netlist.connect(n1, u1a);

	REQUIRE_NOTHROW( netlist.net_remove(n1) );
	REQUIRE( netlist.net_count() == 0 );
	REQUIRE( netlist.pin_net(pi) == openeda::entity::entity() );
	REQUIRE( netlist.pin_net(u1a) == openeda::entity::entity() );

}

TEST_CASE("netlist/insert PI","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);

	auto inp1 = netlist.PI_insert("inp1");
	REQUIRE(netlist.PI_count() == 1);
	REQUIRE(netlist.pin_name(inp1) == "inp1");
	REQUIRE(
			std::find(netlist.PI_begin(), netlist.PI_end(), inp1)
					!= netlist.PI_end());
}

TEST_CASE("netlist/remove PI","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);

	auto inp1 = netlist.PI_insert("inp1");

	netlist.PI_remove(inp1);

	REQUIRE(netlist.PI_count() == 0);
	REQUIRE_THROWS(netlist.pin_name(inp1));
	REQUIRE(
			std::find(netlist.PI_begin(), netlist.PI_end(), inp1)
					== netlist.PI_end());
}

TEST_CASE("netlist/insert 2 PI and remove 1","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);

	auto inp1 = netlist.PI_insert("inp1");
	auto inp2 = netlist.PI_insert("inp2");

	netlist.PI_remove(inp1);

	REQUIRE(netlist.PI_count() == 1);
	REQUIRE_THROWS(netlist.pin_name(inp1));
	REQUIRE(
			std::find(netlist.PI_begin(), netlist.PI_end(), inp1)
					== netlist.PI_end());
	REQUIRE(
			std::find(netlist.PI_begin(), netlist.PI_end(), inp2)
					!= netlist.PI_end());
}


TEST_CASE("netlist/insert PO","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);

	auto out = netlist.PO_insert("out");

	REQUIRE(netlist.PO_count() == 1);
	REQUIRE(
			std::find(netlist.PO_begin(), netlist.PO_end(), out)
					!= netlist.PO_end());
}


TEST_CASE("netlist/remove PO","[netlist]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);

	auto out = netlist.PO_insert("out");

	REQUIRE_NOTHROW( netlist.PO_remove(out) );
	REQUIRE(netlist.PO_count() == 0);
	REQUIRE(
			std::find(netlist.PO_begin(), netlist.PO_end(), out)
					== netlist.PO_end());
}

