#include "../catch.hpp"

#include "../timing/liberty.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("liberty/pin capacitance", "[timing][liberty]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::timing::library_timing_arcs tarcs { &std_cells };
	openeda::timing::library lib { &tarcs, &std_cells };
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);
	REQUIRE(lib.pin_capacitance(std_cells.pin_create(std_cells.cell_create("NOR2_X4"), "b")) == boost::units::quantity<boost::units::si::capacitance>(3 * boost::units::si::femto * boost::units::si::farads));
	REQUIRE(lib.pin_capacitance(std_cells.pin_create(std_cells.cell_create("NOR2_X4"), "o")) == boost::units::quantity<boost::units::si::capacitance>(0.0 * boost::units::si::femto * boost::units::si::farads));
}

TEST_CASE("liberty/timing arcs", "[timing][liberty]") {
	openeda::standard_cell::standard_cells std_cells;

	auto NOR2_X4 = std_cells.cell_create("NOR2_X4");
	auto NOR2_X4a = std_cells.pin_create(NOR2_X4, "a");
	auto NOR2_X4b = std_cells.pin_create(NOR2_X4, "b");
	auto NOR2_X4o = std_cells.pin_create(NOR2_X4, "o");

	openeda::timing::library_timing_arcs tarcs { &std_cells };
	openeda::timing::library lib { &tarcs, &std_cells };
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);

	auto o_timing_arcs = lib.pin_timing_arcs(NOR2_X4o);

	REQUIRE(o_timing_arcs.size() == 2);

	bool froma { false }, fromb { false };

	for (auto arc : o_timing_arcs) {
		if (lib.timing_arc_from(arc) == NOR2_X4a)
			froma = true;
		else if (lib.timing_arc_from(arc) == NOR2_X4b)
			fromb = true;
		REQUIRE(lib.timing_arc_to(arc) == NOR2_X4o);
	}
	REQUIRE((froma && fromb));

	auto a_timing_arcs = lib.pin_timing_arcs(NOR2_X4a);
	REQUIRE(a_timing_arcs.size() == 1);
	REQUIRE(a_timing_arcs.front() == lib.timing_arc(NOR2_X4a, NOR2_X4o));
	REQUIRE(lib.timing_arc_from(a_timing_arcs.front()) == NOR2_X4a);
	REQUIRE(lib.timing_arc_to(a_timing_arcs.front()) == NOR2_X4o);
	auto b_timing_arcs = lib.pin_timing_arcs(NOR2_X4b);
	REQUIRE(b_timing_arcs.size() == 1);
	REQUIRE(b_timing_arcs.front() == lib.timing_arc(NOR2_X4b, NOR2_X4o));
	REQUIRE(lib.timing_arc_from(b_timing_arcs.front()) == NOR2_X4b);
	REQUIRE(lib.timing_arc_to(b_timing_arcs.front()) == NOR2_X4o);

}

TEST_CASE("liberty/lut", "[timing][liberty]") {
	openeda::standard_cell::standard_cells std_cells;
	auto NOR2_X4 = std_cells.cell_create("NOR2_X4");
	auto NOR2_X4a = std_cells.pin_create(NOR2_X4, "a");
	auto NOR2_X4b = std_cells.pin_create(NOR2_X4, "b");
	auto NOR2_X4o = std_cells.pin_create(NOR2_X4, "o");
	openeda::timing::library_timing_arcs tarcs { &std_cells };
	openeda::timing::library lib { &tarcs, &std_cells };
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);
	auto arc = lib.timing_arc(NOR2_X4a, NOR2_X4o);
	const openeda::timing::library::LUT & fall_delay = lib.timing_arc_fall_delay(arc);
	REQUIRE( (fall_delay.row_count() == 7) );
	REQUIRE(fall_delay.column_count() == 8);
	REQUIRE(fall_delay.at(3, 4) == boost::units::quantity<boost::units::si::time>(52.84 * boost::units::si::pico * boost::units::si::seconds));
	REQUIRE(fall_delay.row_value(2) == boost::units::quantity<boost::units::si::capacitance>(4.00*boost::units::si::femto*boost::units::si::farads));
}


TEST_CASE("liberty/timing sense", "[timing][liberty]") {
	openeda::standard_cell::standard_cells std_cells;
	auto NOR2_X4 = std_cells.cell_create("NOR2_X4");
	auto NOR2_X4a = std_cells.pin_create(NOR2_X4, "a");
	auto NOR2_X4b = std_cells.pin_create(NOR2_X4, "b");
	auto NOR2_X4o = std_cells.pin_create(NOR2_X4, "o");
	openeda::timing::library_timing_arcs tarcs { &std_cells };
	openeda::timing::library lib { &tarcs, &std_cells };
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);
	auto arc = lib.timing_arc(NOR2_X4a, NOR2_X4o);
	REQUIRE( lib.timing_arc_timing_sense(arc) == openeda::timing::unateness::NEGATIVE_UNATE );
}

TEST_CASE("liberty/pin direction", "[timing][liberty]") {
	openeda::standard_cell::standard_cells std_cells;
	auto NOR2_X4 = std_cells.cell_create("NOR2_X4");
	auto NOR2_X4a = std_cells.pin_create(NOR2_X4, "a");
	auto NOR2_X4b = std_cells.pin_create(NOR2_X4, "b");
	auto NOR2_X4o = std_cells.pin_create(NOR2_X4, "o");
	openeda::timing::library_timing_arcs tarcs { &std_cells };
	openeda::timing::library lib { &tarcs, &std_cells };
	openeda::timing::liberty::read("benchmarks/superblue16/superblue16_Early.lib", lib);
	REQUIRE( std_cells.pin_direction(NOR2_X4a) == openeda::standard_cell::pin_directions::INPUT );
	REQUIRE( std_cells.pin_direction(NOR2_X4b) == openeda::standard_cell::pin_directions::INPUT );
	REQUIRE( std_cells.pin_direction(NOR2_X4o) == openeda::standard_cell::pin_directions::OUTPUT );

}

