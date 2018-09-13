#include <catch.hpp>

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

    auto box_iterator = nand2_x1_geometry.begin();

    auto first_box = *box_iterator;

    using dbu_t = ophidian::placement::Library::unit_type;

    CHECK(first_box.min_corner().x() == dbu_t{0});
    CHECK(first_box.min_corner().y() == dbu_t{0});

    using micron_t = ophidian::parser::Lef::micrometer_type;

    CHECK(first_box.max_corner().x() == dbu_t{ micron_t{1.140} * lef.micrometer_to_dbu_ratio()});
    CHECK(first_box.max_corner().y() == dbu_t{ micron_t{1.71} * lef.micrometer_to_dbu_ratio()});
}
