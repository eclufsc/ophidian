#include <catch2/catch.hpp>

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


TEST_CASE("Make standard cells from an iccad2020 file.", "[circuit][StandardCells][factory]")
{
    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto std_cells = ophidian::circuit::StandardCells{};
    ophidian::circuit::factory::make_standard_cells(std_cells, sample);

    REQUIRE(std_cells.size_cell() == 3);
    REQUIRE(std_cells.size_pin() == 7);

    std::vector<std::string> std_cell_names,
                             std_pin_names,
                             expected_cell_names{"MC1", "MC2", "MC3"},
                             expected_pin_names{"MC1:P1", "MC1:P2",
                                                "MC2:P1", "MC2:P2",
                                                "MC3:P1", "MC3:P2", "MC3:P3"};
    for(auto std_cell : std_cells.range_cell())
        std_cell_names.push_back(std_cells.name(std_cell));
    REQUIRE(std::is_permutation(std_cell_names.begin(),
                                std_cell_names.end(),
                                expected_cell_names.begin()));

    for(auto std_pin : std_cells.range_pin())
        std_cell_names.push_back(std_cells.name(std_pin));
    REQUIRE(std::is_permutation(std_pin_names.begin(),
                                std_pin_names.end(),
                                expected_pin_names.begin()));

    auto cell_mc1 = std_cells.find_cell("MC1");
    REQUIRE(std_cells.name(cell_mc1) == "MC1");
    REQUIRE(std_cells.pins(cell_mc1).size() == 2);
    std::vector<std::string> pin_names,
                             expected_pins{"MC1:P1", "MC1:P2"};
    for(auto pin : std_cells.pins(cell_mc1))
        pin_names.push_back(std_cells.name(pin));
    REQUIRE(std::is_permutation(pin_names.begin(),
                                pin_names.end(),
                                expected_pins.begin()));
    auto std_pin_mc1_p1 = std_cells.find_pin("MC1:P1");
    auto found_mc1 = std_cells.cell(std_pin_mc1_p1);
    REQUIRE(std_cells.name(found_mc1) == "MC1");
}
