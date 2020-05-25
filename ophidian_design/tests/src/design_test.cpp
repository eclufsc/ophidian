#include <catch2/catch.hpp>

#include <ophidian/design/Design.h>
#include <ophidian/util/Units.h>

using namespace ophidian::design;
using dbu_t = ophidian::util::database_unit_t;


TEST_CASE("Design: testing netlist.", "[design]")
{
    Design design;
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
    Design design;
    REQUIRE(design.floorplan().chip_origin().x() == dbu_t{0.0});
    REQUIRE(design.floorplan().chip_origin().y() == dbu_t{0.0});
    REQUIRE(design.floorplan().chip_upper_right_corner().x() == dbu_t{0.0});
    REQUIRE(design.floorplan().chip_upper_right_corner().y() == dbu_t{0.0});
    REQUIRE(design.floorplan().range_site().empty());
    REQUIRE(design.floorplan().range_row().empty());

    auto origin = ophidian::util::LocationDbu{dbu_t{2.0}, dbu_t{3.0}};
    auto boundaries = ophidian::util::LocationDbu{dbu_t{4000.0}, dbu_t{3000.0}};

    design.floorplan().chip_origin() = origin;
    design.floorplan().chip_upper_right_corner() = boundaries;

    REQUIRE(design.floorplan().chip_origin().x() == dbu_t{2.0});
    REQUIRE(design.floorplan().chip_origin().y() == dbu_t{3.0});
    REQUIRE(design.floorplan().chip_upper_right_corner().x() == dbu_t{4000.0});
    REQUIRE(design.floorplan().chip_upper_right_corner().y() == dbu_t{3000.0});

}

TEST_CASE("Design: testing placement.", "[design]")
{
    Design design;
    auto cell1 = design.netlist().add_cell_instance("cell1");
    auto cell2 = design.netlist().add_cell_instance("cell2");

    auto cell1Location = ophidian::util::LocationDbu{ dbu_t{10}, dbu_t{20}};
    auto cell2Location = ophidian::util::LocationDbu{ dbu_t{20}, dbu_t{10}};

    design.placement().place(cell1, cell1Location);
    design.placement().place(cell2, cell2Location);
    design.placement().setOrientation(cell1, ophidian::placement::Placement::Orientation::N);
    design.placement().setOrientation(cell2, ophidian::placement::Placement::Orientation::S);

    REQUIRE(cell1Location.x() == design.placement().location((cell1)).x());
    REQUIRE(cell1Location.y() == design.placement().location((cell1)).y());
    REQUIRE(cell2Location.x() == design.placement().location((cell2)).x());
    REQUIRE(cell2Location.y() == design.placement().location((cell2)).y());
    REQUIRE(ophidian::placement::Placement::Orientation::N == design.placement().orientation(cell1));
    REQUIRE(ophidian::placement::Placement::Orientation::S == design.placement().orientation(cell2));
}

TEST_CASE("Design: testing library (setting geometry of cells).", "[design]")
{
    using geometry_container = ophidian::geometry::CellGeometry::geometry_container_type;
    using box_type = ophidian::geometry::CellGeometry::box_type;

    Design design;
    auto cell1 = design.standard_cells().add_cell("cell1");
    auto cell2 = design.standard_cells().add_cell("cell2");

    box_type box1{{dbu_t{0}, dbu_t{0}}, {dbu_t{1}, dbu_t{1}}};
    geometry_container cell1Boxes{{std::make_pair(box1, "Metal1")}};

    ophidian::geometry::CellGeometry cell1Geometry(cell1Boxes);
    design.placement_library().geometry(cell1) = cell1Geometry;

    box_type box2{{dbu_t{0}, dbu_t{0}}, {dbu_t{20}, dbu_t{10}}};
    geometry_container cell2Boxes{{std::make_pair(box2, "Metal1")}};

    ophidian::geometry::CellGeometry cell2Geometry(cell2Boxes);
    design.placement_library().geometry(cell2) =  cell2Geometry;

    REQUIRE(cell1Geometry == design.placement_library().geometry(cell1));
    REQUIRE(cell2Geometry == design.placement_library().geometry(cell2));
    REQUIRE(design.placement_library().geometry(cell1) != design.placement_library().geometry(cell2));
}

