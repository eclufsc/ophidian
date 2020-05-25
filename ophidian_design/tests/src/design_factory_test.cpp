#include <catch.hpp>

#include <ophidian/design/DesignFactory.h>

TEST_CASE("Design factory: populate with simple lef, def, verilog.", "[design][Design][factory]")
{
    using geometry_container = ophidian::geometry::CellGeometry::geometry_container_type;
    using box_type = ophidian::geometry::CellGeometry::box_type;

    auto lef = ophidian::parser::Lef{"input_files/simple/simple.lef"};
    auto def = ophidian::parser::Def{"input_files/simple/simple.def"};
    auto verilog = ophidian::parser::Verilog{"input_files/simple/simple.v"};

    auto design = ophidian::design::Design{};

    ophidian::design::factory::make_design(design, def, lef, verilog);

    auto cell_u1 = design.netlist().find_cell_instance("u1");

    using dbu_t = ophidian::placement::Placement::unit_type;

    auto cell_u1_xpos = design.placement().location(cell_u1).x();
    auto cell_u1_ypos = design.placement().location(cell_u1).y();

    CHECK(cell_u1_xpos == dbu_t{3420});
    CHECK(cell_u1_ypos == dbu_t{6840});

    auto std_cell_u1 = design.netlist().std_cell(cell_u1);

    auto std_cell_u1_geometry = design.placement_library().geometry(std_cell_u1);

    auto first_box = std_cell_u1_geometry.front().first;

    CHECK(first_box.min_corner().x() == dbu_t{0});
    CHECK(first_box.min_corner().y() == dbu_t{0});

    using micron_t = ophidian::parser::Lef::micrometer_type;

    CHECK(first_box.max_corner().x() == dbu_t{ micron_t{1.140} * lef.micrometer_to_dbu_ratio()});
    CHECK(first_box.max_corner().y() == dbu_t{ micron_t{1.71} * lef.micrometer_to_dbu_ratio()});

    auto cell_u1_placed_geometry = design.placement().geometry(cell_u1);

    CHECK(cell_u1_placed_geometry.front().first.min_corner().x() == cell_u1_xpos);
    CHECK(cell_u1_placed_geometry.front().first.min_corner().y() == cell_u1_ypos);

    CHECK(cell_u1_placed_geometry.front().first.max_corner().x() == std_cell_u1_geometry.front().first.max_corner().x() + cell_u1_xpos);
    CHECK(cell_u1_placed_geometry.front().first.max_corner().y() == std_cell_u1_geometry.front().first.max_corner().y() + cell_u1_ypos);
}

TEST_CASE("Design factory: test design_factory_iccad17 with contest files", "[design][Design][factory]")
{
    auto lef = ophidian::parser::Lef{};

    lef.read_file("input_files/iccad17/pci_bridge32_a_md1/tech.lef");
    lef.read_file("input_files/iccad17/pci_bridge32_a_md1/cells_modified.lef");

    auto def = ophidian::parser::Def{};

    def.read_file("input_files/iccad17/pci_bridge32_a_md1/placed.def");

    auto design = ophidian::design::Design{};

    ophidian::design::factory::make_design_iccad2017(design, def, lef);

    CHECK(design.netlist().size_cell_instance() == 29521);
}
