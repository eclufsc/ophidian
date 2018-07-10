#include <catch.hpp>

#include <ophidian/design/Design.h>
#include <ophidian/util/Units.h>

using namespace ophidian::design;
using dbu_t = ophidian::util::database_unit_t;

Design design;

TEST_CASE("Design: testing netlist.", "[design]")
{
	auto cell = design.netlist().add_cell_instance("cell");
	auto pin = design.netlist().add_pin_instance("pin");
	auto net = design.netlist().add_net("net");
	REQUIRE(design.netlist().size_cell_instance() == 1);
	REQUIRE(design.netlist().size_pin_instance() == 1);
	REQUIRE(design.netlist().size_net() == 1);
	REQUIRE(design.netlist().size_input_pad() == 0 );
	REQUIRE(design.netlist().size_output_pad() == 0 );

}

TEST_CASE("Design: testing floorplan.", "[design]")
{
	REQUIRE(design.floorplan().chipOrigin().x() == dbu_t{0.0});
	REQUIRE(design.floorplan().chipOrigin().y() == dbu_t{0.0});
	REQUIRE(design.floorplan().chipUpperRightCorner().x() == dbu_t{0.0});
	REQUIRE(design.floorplan().chipUpperRightCorner().y() == dbu_t{0.0});
	REQUIRE(design.floorplan().sitesRange().empty());
	REQUIRE(design.floorplan().rowsRange().empty());

	auto origin = ophidian::util::LocationDbu{dbu_t{2.0}, dbu_t{3.0}};
	auto boundaries = ophidian::util::LocationDbu{dbu_t{4000.0}, dbu_t{3000.0}};

	design.floorplan().chipOrigin(origin);
	design.floorplan().chipUpperRightCorner(boundaries);
	REQUIRE(design.floorplan().chipOrigin().x() == dbu_t{2.0});
	REQUIRE(design.floorplan().chipOrigin().y() == dbu_t{3.0});
	REQUIRE(design.floorplan().chipUpperRightCorner().x() == dbu_t{4000.0});
	REQUIRE(design.floorplan().chipUpperRightCorner().y() == dbu_t{3000.0});

}

TEST_CASE("Design: testing placement.", "[design]")
{
	auto cell1 = design.netlist().add_cell_instance("cell1");
	auto cell2 = design.netlist().add_cell_instance("cell2");

	auto cell1Location = ophidian::util::LocationDbu{ dbu_t{10}, dbu_t{20}};
	auto cell2Location = ophidian::util::LocationDbu{ dbu_t{20}, dbu_t{10}};

	design.placement().placeCell(cell1, cell1Location);
	design.placement().placeCell(cell2, cell2Location);

	REQUIRE(cell1Location.x() == design.placement().cellLocation((cell1)).x());
	REQUIRE(cell1Location.y() == design.placement().cellLocation((cell1)).y());
	REQUIRE(cell2Location.x() == design.placement().cellLocation((cell2)).x());
	REQUIRE(cell2Location.y() == design.placement().cellLocation((cell2)).y());
}

TEST_CASE("Design: testing library (setting geometry of cells).", "[design]")
{
    auto cell1 = design.standard_cells().add_cell("cell1");
    auto cell2 = design.standard_cells().add_cell("cell2");

	std::vector<ophidian::geometry::Box<dbu_t>> cell1Boxes = {
        ophidian::geometry::Box<dbu_t>{ophidian::geometry::Point<dbu_t>{dbu_t{0}, dbu_t{0}}, ophidian::geometry::Point<dbu_t>{dbu_t{10}, dbu_t{10}}}
    };
	ophidian::geometry::MultiBox<dbu_t> cell1Geometry(cell1Boxes);
	design.library().connect(cell1, cell1Geometry);

	std::vector<ophidian::geometry::Box<dbu_t>> cell2Boxes = {
        ophidian::geometry::Box<dbu_t>{
            ophidian::geometry::Point<dbu_t>{dbu_t{0}, dbu_t{0}},
            ophidian::geometry::Point<dbu_t>{dbu_t{20}, dbu_t{10}}
        }
    };
	ophidian::geometry::MultiBox<dbu_t> cell2Geometry(cell2Boxes);
	design.library().connect(cell2, cell2Geometry);

	REQUIRE(cell1Geometry == design.library().geometry(cell1));
	REQUIRE(cell2Geometry == design.library().geometry(cell2));
	REQUIRE(design.library().geometry(cell1) != design.library().geometry(cell2));
}

