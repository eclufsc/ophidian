#include "../catch.hpp"

#include <standard_cells.h>

using namespace openeda::standard_cell;

TEST_CASE("standard cells/ empty ", "[standard_cell]") {

	standard_cells std_cells;
	REQUIRE( std_cells.empty() );
	REQUIRE( std_cells.size() == 0 );

}


TEST_CASE("standard cells/ create ", "[standard_cell]") {

	standard_cells std_cells;

	auto e = std_cells.create("INVX1");

	REQUIRE( std_cells.size() == 1 );
	REQUIRE( std_cells.name(e) == "INVX1" );

}


TEST_CASE("standard cells/ unique name ", "[standard_cell]") {

	standard_cells std_cells;

	auto e1 = std_cells.create("INVX1");
	auto e2 = std_cells.create("INVX1");

	REQUIRE( e1.id() == e2.id() );

	REQUIRE( std_cells.size() == 1 );
	REQUIRE( std_cells.name(e1) == std_cells.name(e2) );

}
