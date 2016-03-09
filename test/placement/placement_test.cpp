#include "../catch.hpp"

#include "../placement/placement.h"

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include  <iostream>

TEST_CASE("placement/place pads", "[placement]") {

	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	auto inp1 = netlist.PI_insert("inp1");
	auto out = netlist.PO_insert("out");

	openeda::placement::library lib { &std_cells };
	openeda::placement::placement placement { &netlist, &lib };
	openeda::geometry::point<double> position { 1.0, 2.0 };
	openeda::geometry::point<double> position2 { 3.0, 4.0 };
	placement.pad_position(inp1, position);
	placement.pad_position(out, position2);

	REQUIRE(placement.pin_position(inp1).x() == position.x());
	REQUIRE(placement.pin_position(inp1).y() == position.y());
	REQUIRE(placement.pin_position(out).x() == position2.x());
	REQUIRE(placement.pin_position(out).y() == position2.y());

}

TEST_CASE("placement/place cell", "[placement]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib { &std_cells };
	openeda::placement::placement placement { &netlist, &lib };

	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");

	lib.pin_offset(INV_X1a, { 1.0, 2.0 });
	lib.pin_offset(INV_X1o, { 3.0, 4.0 });

	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u1a = netlist.pin_insert(u1, "a");
	auto u1o = netlist.pin_insert(u1, "o");

	placement.cell_position(u1, { 100.0, 200.0 });

	REQUIRE(placement.pin_position(u1a).x() == 101.0);
	REQUIRE(placement.pin_position(u1a).y() == 202.0);

	REQUIRE(placement.pin_position(u1o).x() == 103.0);
	REQUIRE(placement.pin_position(u1o).y() == 204.0);

}

TEST_CASE("placement/hpwl test", "[placement][hpwl]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::netlist::netlist netlist(&std_cells);
	openeda::placement::library lib { &std_cells };

	// create lib
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X2 = std_cells.cell_create("INV_X2");
	auto INV_X3 = std_cells.cell_create("INV_X3");
	auto INV_X4 = std_cells.cell_create("INV_X4");

	auto INV_X1_z = std_cells.pin_create(INV_X1, "z");
	auto INV_X2_a = std_cells.pin_create(INV_X2, "a");
	auto INV_X3_a = std_cells.pin_create(INV_X3, "a");
	auto INV_X4_a = std_cells.pin_create(INV_X4, "a");

	using point = openeda::geometry::point<double>;
	using polygon = openeda::geometry::polygon<point>;
	using multipolygon = openeda::geometry::multi_polygon< polygon >;

	auto X1_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 128.0,
			200.0 }, point { 128.0, 0.0 }, point { 0.0, 0.0 } };

	auto X2_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 256.0,
				200.0 }, point { 256.0, 0.0 }, point { 0.0, 0.0 } };
	auto X3_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 384.0,
				200.0 }, point { 384.0, 0.0 }, point { 0.0, 0.0 } };
	auto X4_points { point { 0.0, 0.0 }, point { 0.0, 200.0 }, point { 512.0,
				200.0 }, point { 512.0, 0.0 }, point { 0.0, 0.0 } };


	multipolygon X1 { polygon { X1_points } };
	multipolygon X2 { polygon { X2_points } };
	multipolygon X3 { polygon { X3_points } };
	multipolygon X4 { polygon { X4_points } };

	lib.geometry(INV_X1, X1);
	lib.geometry(INV_X2, X2);
	lib.geometry(INV_X3, X3);
	lib.geometry(INV_X4, X4);

	lib.pin_offset(INV_X1_z, {80.0, 50.0});
	lib.pin_offset(INV_X2_a, {10.0, 60.0});
	lib.pin_offset(INV_X3_a, {5.0, 40.0});
	lib.pin_offset(INV_X4_a, {7.0, 45.0});

	openeda::placement::placement placement { &netlist, &lib };

	auto u1 = netlist.cell_insert("u1", "INV_X1");
	auto u2 = netlist.cell_insert("u2", "INV_X2");
	auto u3 = netlist.cell_insert("u3", "INV_X3");
	auto u4 = netlist.cell_insert("u4", "INV_X4");

	auto u1_z = netlist.pin_insert(u1, "z");
	auto u2_a = netlist.pin_insert(u2, "a");
	auto u3_a = netlist.pin_insert(u3, "a");
	auto u4_a = netlist.pin_insert(u4, "a");

	auto n1 = netlist.net_insert("n1");
	netlist.connect(n1, u1_z);
	netlist.connect(n1, u2_a);
	netlist.connect(n1, u3_a);
	netlist.connect(n1, u4_a);

	placement.cell_position(u1, {0.0, 0.0});
	placement.cell_position(u2, {130.0, 200.0});
	placement.cell_position(u3, {400.0, 0.0});
	placement.cell_position(u4, {200.0, 400.0});

	auto net_pins = netlist.net_pins(n1);
	std::vector<double> X(net_pins.size());
	std::vector<double> Y(net_pins.size());
	X.resize(0);
	Y.resize(0);
	for(auto pin : net_pins)
	{
		auto pos = placement.pin_position(pin);
		X.push_back(pos.x());
		Y.push_back(pos.y());
	}

	point lower{*std::min_element(X.begin(), X.end()), *std::min_element(Y.begin(), Y.end())};
	point upper{*std::max_element(X.begin(), X.end()), *std::max_element(Y.begin(), Y.end())};
	double HPWL = openeda::geometry::manhattan_distance(lower, upper);
	REQUIRE( HPWL == 730.0 );

}
