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

#include <standard_cells.h>
#include "../standard_cell/cells.h"
#include "../standard_cell/pins.h"

using namespace ophidian::standard_cell;

TEST_CASE("standard cells/cells", "[standard_cells][cells]")
{
	ophidian::entity::system cells_system;
	cells cells(cells_system);
	auto cell = cells_system.create();
	cells.name(cell, "the cell");
	REQUIRE( cells.name(cell) == "the cell" );
	REQUIRE( cells.pins(cell).empty() );
}

TEST_CASE("standard cells/pins", "[standard_cells][pins]")
{
	ophidian::entity::system cells_system;
	ophidian::entity::system pins_system;
	cells std_cells(cells_system);
	pins std_cells_pins(pins_system);
	auto INV_X1 = cells_system.create();
	std_cells.name(INV_X1, "INV_X1");
	auto INV_X1_a = pins_system.create();
	std_cells_pins.name(INV_X1_a, "INV_X1:a");
	std_cells_pins.owner(INV_X1_a, INV_X1);
	std_cells.insert_pin(INV_X1, INV_X1_a);
	auto cell_pins = std_cells.pins(INV_X1);
	REQUIRE( std::find(cell_pins.begin(), cell_pins.end(), INV_X1_a) != cell_pins.end() );
	REQUIRE( std_cells_pins.owner(INV_X1_a) == INV_X1 );

}

TEST_CASE("standard cells/ empty ", "[standard_cell]") {

	standard_cells std_cells;
	REQUIRE( std_cells.cell_count() == 0 );
	REQUIRE( std_cells.pin_count() == 0 );

}


TEST_CASE("standard cells/ create ", "[standard_cell]") {

	standard_cells std_cells;

	auto e = std_cells.cell_create("INVX1");

	REQUIRE( std_cells.cell_count() == 1 );
	REQUIRE( std_cells.cell_name(e) == "INVX1" );

}


TEST_CASE("standard cells/ unique name ", "[standard_cell]") {

	standard_cells std_cells;

	auto e1 = std_cells.cell_create("INVX1");
	auto e2 = std_cells.cell_create("INVX1");

	REQUIRE( e1.id() == e2.id() );

	REQUIRE( std_cells.cell_count() == 1 );
	REQUIRE( std_cells.cell_name(e1) == std_cells.cell_name(e2) );

}

TEST_CASE("standard cells/add pin", "[standard_cell]")
{
	standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1_a = std_cells.pin_create(INV_X1, "a");

	REQUIRE( std_cells.pin_owner(INV_X1_a) == INV_X1 );
	auto cell_pins = std_cells.cell_pins(INV_X1);


	REQUIRE( std_cells.pin_name(INV_X1_a) == "INV_X1:a" );

	REQUIRE( std::find(cell_pins.begin(), cell_pins.end(), INV_X1_a) != cell_pins.end() );

}

TEST_CASE("standard cells/add pin twice", "[standard_cell]")
{
	standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1_a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1_a2 = std_cells.pin_create(INV_X1, "a");
	REQUIRE( INV_X1_a2 == INV_X1_a );
	REQUIRE( std_cells.cell_pins(INV_X1).size() == 1 );
}



TEST_CASE("standard cells/pin direction", "[standard_cell]")
{
	standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1_a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1_o = std_cells.pin_create(INV_X1, "o");
	REQUIRE( std_cells.pin_direction(INV_X1_a) == ophidian::standard_cell::pin_directions::NOT_ASSIGNED );
	std_cells.pin_direction(INV_X1_a, ophidian::standard_cell::pin_directions::INPUT);
	std_cells.pin_direction(INV_X1_o, ophidian::standard_cell::pin_directions::OUTPUT);
	REQUIRE( std_cells.pin_direction(INV_X1_a) == ophidian::standard_cell::pin_directions::INPUT );
	REQUIRE( std_cells.pin_direction(INV_X1_o) == ophidian::standard_cell::pin_directions::OUTPUT );
	REQUIRE( std_cells.pin_direction(INV_X1_a) != ophidian::standard_cell::pin_directions::NOT_ASSIGNED );
	REQUIRE( std_cells.pin_direction(INV_X1_o) != ophidian::standard_cell::pin_directions::NOT_ASSIGNED );
}
