#include <catch2/catch.hpp>

#include <ophidian/circuit/StandardCells.h>
#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/placement/LibraryFactory.h>

TEST_CASE("Library factory: populate with simple lef.", "[circuit][Netlist][factory]")
{
    auto std_cells = ophidian::circuit::StandardCells{};

    auto lef = ophidian::parser::Lef{"input_files/simple/simple.lef"};

    ophidian::circuit::factory::make_standard_cells(std_cells, lef);

    auto library = ophidian::placement::Library{std_cells};

    ophidian::placement::factory::make_library(library, lef, std_cells);

    auto nand2_x1 = std_cells.find_cell("NAND2_X1");

    auto nand2_x1_geometry = library.geometry(nand2_x1);

    CHECK(nand2_x1_geometry.size() == 1);

    auto geometry_iterator = nand2_x1_geometry.begin();

    auto first_box = geometry_iterator->first;

    using dbu_t = ophidian::placement::Library::unit_type;

    CHECK(first_box.min_corner().x() == dbu_t{0});
    CHECK(first_box.min_corner().y() == dbu_t{0});

    using micron_t = ophidian::parser::Lef::micrometer_type;

    auto max_x = dbu_t{ micron_t{1.140} * lef.micrometer_to_dbu_ratio()};
    auto max_y = dbu_t{ micron_t{1.71} * lef.micrometer_to_dbu_ratio()};

    CHECK(first_box.max_corner().x() == max_x);
    CHECK(first_box.max_corner().y() == max_y);
}

TEST_CASE("Make a placement library from an iccad2020 file.", "[placement][Placement][factory]")
{
    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto std_cells = ophidian::circuit::StandardCells{};
    ophidian::circuit::factory::make_standard_cells(std_cells, sample);

    auto library = ophidian::placement::Library{std_cells};
    ophidian::placement::factory::make_library(library, sample, std_cells);

    auto pin_p1 = std_cells.find_pin("MC3:P1");
    auto geometries_p1 = library.geometry(pin_p1);
    auto geometry_p1 = geometries_p1.front();
    auto p1_layer_name = geometry_p1.second;
    REQUIRE(p1_layer_name == "M2");

    auto pin_p2 = std_cells.find_pin("MC3:P2");
    auto geometries_p2 = library.geometry(pin_p2);
    auto geometry_p2 = geometries_p2.front();
    auto p2_layer_name = geometry_p2.second;
    REQUIRE(p2_layer_name == "M1");
}
