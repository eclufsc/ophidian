/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../catch.hpp"

#include "../timing/library_timing_arcs.h"

TEST_CASE("library timing arc/empty", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	REQUIRE(tarcs.size() == 0);
}

TEST_CASE("library timing arc/create, check from & to", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.size() == 1);
	REQUIRE(tarcs.from(arc) == INV_X1a);
	REQUIRE(tarcs.to(arc) == INV_X1o);
}

TEST_CASE("library timing arc/assign pins to timing arc", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	auto arcs = tarcs.pin_timing_arcs(INV_X1a);
	REQUIRE(arcs.size() == 1);
	REQUIRE(arcs.front() == arc);
	auto arcs2 = tarcs.pin_timing_arcs(INV_X1o);
	REQUIRE(arcs2 == arcs);
}

TEST_CASE("library timing arc/get", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.get(INV_X1a, INV_X1o) == arc);
}
