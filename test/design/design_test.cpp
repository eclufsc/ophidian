#include "design_test.h"
#include <catch.hpp>

#include <ophidian/design/Design.h>

using namespace ophidian::design;

Design design;

TEST_CASE("Design: testing netlist.", "[design]")
{
	auto cell = design.netlist().add(ophidian::circuit::Cell(), "cell");
	auto pin = design.netlist().add(ophidian::circuit::Pin(), "pin");
	auto net = design.netlist().add(ophidian::circuit::Net(), "net");
	REQUIRE(design.netlist().size(ophidian::circuit::Cell()) == 1);
	REQUIRE(design.netlist().size(ophidian::circuit::Pin()) == 1);
	REQUIRE(design.netlist().size(ophidian::circuit::Net()) == 1);
	REQUIRE(design.netlist().size(ophidian::circuit::Input()) == 0 );
	REQUIRE(design.netlist().size(ophidian::circuit::Output()) == 0 );

}

TEST_CASE("Design: testing floorplan.", "[design]")
{
	ophidian::util::LocationMicron origin(0.0, 0.0);
	ophidian::util::LocationMicron boundaries(0.0, 0.0);

	REQUIRE(design.floorplan().chipOrigin() == origin);
	REQUIRE(design.floorplan().chipUpperRightCorner() == boundaries);
	REQUIRE(design.floorplan().sitesRange().empty());
	REQUIRE(design.floorplan().rowsRange().empty());

	origin = ophidian::util::LocationMicron(2.0, 3.0);
	boundaries = ophidian::util::LocationMicron(4000.0, 3000.0);
	design.floorplan().chipOrigin(origin);
	design.floorplan().chipUpperRightCorner(boundaries);
	REQUIRE(design.floorplan().chipOrigin() == origin);
	REQUIRE(design.floorplan().chipUpperRightCorner() == boundaries);

}

TEST_CASE("Design: testing placement.", "[design]")
{
	auto cell1 = design.netlist().add(ophidian::circuit::Cell(), "cell1");
	auto cell2 = design.netlist().add(ophidian::circuit::Cell(), "cell2");
	ophidian::util::LocationMicron cell1Location(10,20);
	ophidian::util::LocationMicron cell2Location(20,10);
	design.placement().placeCell(cell1, cell1Location);
	design.placement().placeCell(cell2, cell2Location);

	REQUIRE(cell1Location == design.placement().location((cell1)));
	REQUIRE(cell2Location == design.placement().location((cell2)));
}

TEST_CASE("Design: testing library (setting geometry of cells).", "[design]")
{
	auto cell1 = design.standardCells().add(ophidian::standard_cell::Cell(), "cell1");
	auto cell2 = design.standardCells().add(ophidian::standard_cell::Cell(), "cell2");

	std::vector<ophidian::geometry::Box> cell1Boxes = {ophidian::geometry::Box(ophidian::geometry::Point(0, 0), ophidian::geometry::Point(10, 10))};
	ophidian::geometry::MultiBox cell1Geometry(cell1Boxes);
	design.library().geometry(cell1, cell1Geometry);

	std::vector<ophidian::geometry::Box> cell2Boxes = {ophidian::geometry::Box(ophidian::geometry::Point(0, 0), ophidian::geometry::Point(20, 10))};
	ophidian::geometry::MultiBox cell2Geometry(cell2Boxes);
	design.library().geometry(cell2, cell2Geometry);

	REQUIRE(cell1Geometry == design.library().geometry(cell1));
	REQUIRE(cell2Geometry == design.library().geometry(cell2));
	REQUIRE(design.library().geometry(cell1) != design.library().geometry(cell2));

}

