#include <catch.hpp>

#include <ophidian/circuit/StandardCellsFactory.h>

TEST_CASE("StandardCells factory: make Simple standard cells.", "[circuit][StandardCells][factory]")
{
    auto std_cells = ophidian::circuit::StandardCells{};

    auto lef = ophidian::parser::Lef{"input_files/simple/simple.lef"};

    ophidian::circuit::factory::make_standard_cells(std_cells, lef);

    CHECK(std_cells.size_cell() == 212);

    CHECK(std_cells.size_pin() == 784);

    auto cell_inv_x1 = std_cells.find_cell("INV_X1");
    
    CHECK(std_cells.name(cell_inv_x1) == "INV_X1");

    auto cell_inv_x1_pins = std_cells.pins(cell_inv_x1);

    CHECK(cell_inv_x1_pins.size() == 2);

    auto pin_iterator = cell_inv_x1_pins.begin();

    auto pin_inv_x1_a = *pin_iterator;

    CHECK(std_cells.name(pin_inv_x1_a) == "INV_X1:a");

    ++pin_iterator;

    auto pin_inv_x1_o = *pin_iterator;

    CHECK(std_cells.name(pin_inv_x1_o) == "INV_X1:o");

    ++pin_iterator;

    CHECK(pin_iterator == cell_inv_x1_pins.end());
}
