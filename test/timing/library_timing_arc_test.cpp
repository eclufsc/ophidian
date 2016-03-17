#include "../catch.hpp"

#include "../timing/library_timing_arcs.h"

TEST_CASE("library timing arc/empty", "[timing][librarytimingarc]") {
	openeda::standard_cell::standard_cells std_cells;
	openeda::timing::library_timing_arcs tarcs(&std_cells);
	REQUIRE(tarcs.size() == 0);
}

TEST_CASE("library timing arc/create, check from & to", "[timing][librarytimingarc]") {
	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	openeda::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.size() == 1);
	REQUIRE(tarcs.from(arc) == INV_X1a);
	REQUIRE(tarcs.to(arc) == INV_X1o);
}

#include "../timing/lookup_table.h"
TEST_CASE("library timing arc/rise fall delay and slew", "[timing][librarytimingarc]") {
	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	openeda::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	openeda::timing::library_timing_arcs::LUT rise_delay(1, 2);
	openeda::timing::library_timing_arcs::LUT fall_delay(2, 1);
	openeda::timing::library_timing_arcs::LUT rise_slew(3, 4);
	openeda::timing::library_timing_arcs::LUT fall_slew(4, 3);
	tarcs.rise_delay(arc, rise_delay);
	tarcs.fall_delay(arc, fall_delay);
	tarcs.rise_slew(arc, rise_slew);
	tarcs.fall_slew(arc, fall_slew);
	REQUIRE(tarcs.rise_delay(arc) == rise_delay);
	REQUIRE(tarcs.fall_delay(arc) == fall_delay);
	REQUIRE(tarcs.rise_slew(arc) == rise_slew);
	REQUIRE(tarcs.fall_slew(arc) == fall_slew);
}

TEST_CASE("library timing arc/assign pins to timing arc", "[timing][librarytimingarc]") {
	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	openeda::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	auto arcs = tarcs.pin_timing_arcs(INV_X1a);
	REQUIRE(arcs.size() == 1);
	REQUIRE(arcs.front() == arc);
	auto arcs2 = tarcs.pin_timing_arcs(INV_X1o);
	REQUIRE(arcs2 == arcs);
}

TEST_CASE("library timing arc/get", "[timing][librarytimingarc]") {
	openeda::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	openeda::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.get(INV_X1a, INV_X1o) == arc);
}
