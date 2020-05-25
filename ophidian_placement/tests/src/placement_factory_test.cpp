#include <catch2/catch.hpp>

#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/circuit/NetlistFactory.h>
#include <ophidian/placement/LibraryFactory.h>
#include <ophidian/placement/PlacementFactory.h>

TEST_CASE("Placement factory: populate with simple project.", "[placement][Placement][factory]")
{
    auto std_cells = ophidian::circuit::StandardCells{};

    auto lef = ophidian::parser::Lef{"input_files/simple/simple.lef"};

    ophidian::circuit::factory::make_standard_cells(std_cells, lef);

    auto library = ophidian::placement::Library{std_cells};

    ophidian::placement::factory::make_library(library, lef, std_cells);

    auto netlist = ophidian::circuit::Netlist{};

    auto verilog = ophidian::parser::Verilog{"input_files/simple/simple.v"};

    ophidian::circuit::factory::make_netlist(netlist, verilog, std_cells);

    auto placement = ophidian::placement::Placement{netlist, library};

    auto def = ophidian::parser::Def{"input_files/simple/simple.def"};

    ophidian::placement::factory::make_placement(placement, def, netlist);

    auto cell_u1 = netlist.find_cell_instance("u1");
    auto cell_f1 = netlist.find_cell_instance("f1");

    using dbu_t = ophidian::placement::Placement::unit_type;

    auto cell_u1_xpos = placement.location(cell_u1).x();
    auto cell_u1_ypos = placement.location(cell_u1).y();

    CHECK(cell_u1_xpos == dbu_t{3420});
    CHECK(cell_u1_ypos == dbu_t{6840});

    CHECK(!placement.isFixed(cell_u1));
    CHECK(placement.isFixed(cell_f1));

    auto std_cell_u1 = netlist.std_cell(cell_u1);

    auto std_cell_u1_geometry = library.geometry(std_cell_u1);

    auto first_box = std_cell_u1_geometry.front().first;

    CHECK(first_box.min_corner().x() == dbu_t{0});
    CHECK(first_box.min_corner().y() == dbu_t{0});

    using micron_t = ophidian::parser::Lef::micrometer_type;

    CHECK(first_box.max_corner().x() == dbu_t{ micron_t{1.140} * lef.micrometer_to_dbu_ratio()});
    CHECK(first_box.max_corner().y() == dbu_t{ micron_t{1.71} * lef.micrometer_to_dbu_ratio()});

    auto cell_u1_placed_geometry = placement.geometry(cell_u1);

    CHECK(cell_u1_placed_geometry.front().first.min_corner().x() == std_cell_u1_geometry.front().first.min_corner().x() + cell_u1_xpos);
    CHECK(cell_u1_placed_geometry.front().first.min_corner().y() == std_cell_u1_geometry.front().first.min_corner().y() + cell_u1_ypos);

    CHECK(cell_u1_placed_geometry.front().first.max_corner().x() == std_cell_u1_geometry.front().first.max_corner().x() + cell_u1_xpos);
    CHECK(cell_u1_placed_geometry.front().first.max_corner().y() == std_cell_u1_geometry.front().first.max_corner().y() + cell_u1_ypos);
}

TEST_CASE("Make a placement from an iccad2020 file.", "[placement][Placement][factory]")
{

    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto std_cells = ophidian::circuit::StandardCells{};
    auto netlist = ophidian::circuit::Netlist{};
    ophidian::circuit::factory::make_standard_cells(std_cells, sample);
    ophidian::circuit::factory::make_netlist(netlist, sample, std_cells);

    auto library = ophidian::placement::Library{std_cells};
    auto placement = ophidian::placement::Placement{netlist, library};
    ophidian::placement::factory::make_placement(placement, sample, netlist);

    auto inst_C2 = netlist.find_cell_instance("C2");
    auto inst_position = placement.location(inst_C2);
    auto position = ophidian::parser::Component::database_unit_point_type{ophidian::parser::Component::database_unit_type{15},
                                                                          ophidian::parser::Component::database_unit_type{45}};
    REQUIRE(position.x() == inst_position.x());
    REQUIRE(position.y() == inst_position.y());
    REQUIRE(placement.isFixed(inst_C2) == false);

    auto inst_C8 = netlist.find_cell_instance("C8");
    auto inst_position2 = placement.location(inst_C8);
    auto position2 = ophidian::parser::Component::database_unit_point_type{ophidian::parser::Component::database_unit_type{45},
                                                                          ophidian::parser::Component::database_unit_type{25}};
    REQUIRE(position2.x() == inst_position2.x());
    REQUIRE(position2.y() == inst_position2.y());
    REQUIRE(placement.isFixed(inst_C8) == false);
}
