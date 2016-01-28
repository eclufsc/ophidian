#include "../catch.hpp"

#include <entity.h>
#include <cells.h>

TEST_CASE("cells/create and destroy","[netlist][cells]")
{
	openeda::entity::system system;
	openeda::netlist::cells cells(&system);

	openeda::standard_cell::standard_cells std_cells;
	auto NAND2_X2 = std_cells.create("NAND2_X2");

	auto u1 = system.create();
	cells.name(u1, "u1");
	cells.standard_cell(u1, NAND2_X2);

	REQUIRE( std_cells.name(cells.standard_cell(u1)) == "NAND2_X2" );
	REQUIRE( cells.pins(u1).empty() );

	system.destroy(u1);

	REQUIRE_THROWS( cells.name(u1) );
	REQUIRE_THROWS( cells.standard_cell(u1) );
	REQUIRE_THROWS( cells.pins(u1) );

}


TEST_CASE("cells/create two","[netlist][cells]")
{
	openeda::entity::system system;
	openeda::netlist::cells cells(&system);

	openeda::standard_cell::standard_cells std_cells;
	auto NAND2_X2 = std_cells.create("NAND2_X2");
	auto INV_X1 = std_cells.create("INV_X1");

	auto u1 = system.create();
	auto u2 = system.create();

	cells.name(u1, "u1");
	cells.standard_cell(u1, NAND2_X2);

	cells.name(u2, "u2");
	cells.standard_cell(u2, INV_X1);

	REQUIRE( std_cells.name(cells.standard_cell(u1)) == "NAND2_X2" );
	REQUIRE( std_cells.name(cells.standard_cell(u2)) == "INV_X1" );

	system.destroy(u1);

	REQUIRE_THROWS( cells.name(u1) );
	REQUIRE_THROWS( cells.standard_cell(u1) );
	REQUIRE_THROWS( cells.pins(u1) );

	REQUIRE_NOTHROW( cells.name(u2) == "u1" );
	REQUIRE_NOTHROW( cells.standard_cell(u2) );
	REQUIRE_NOTHROW( cells.pins(u2).empty() );

}
