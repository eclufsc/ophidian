#include <catch.hpp>

#include <ophidian/circuit/NetlistFactory.h>
#include <ophidian/circuit/StandardCellsFactory.h>

using namespace ophidian::circuit;

TEST_CASE("Netlist factory: populate with simple verilog.", "[circuit][Netlist][factory]")
{
    auto netlist = Netlist{};

    auto simple = ophidian::parser::Verilog{"input_files/simple/simple.v"};

    factory::make_netlist(netlist, simple);

    CHECK(netlist.size_cell_instance() == 6);

    CHECK(netlist.size_pin_instance() == 19);
    
    CHECK(netlist.size_net() == 9);

    CHECK(netlist.size_input_pad() == 3);

    CHECK(netlist.size_output_pad() == 1);

    auto cell_u1 = netlist.find_cell_instance("u1");

    CHECK(netlist.name(cell_u1) == "u1");

    auto cell_u1_pins = netlist.pins(cell_u1);

    CHECK(cell_u1_pins.size() == 3);

    auto pin_iterator = cell_u1_pins.begin();

    auto pin_u1_o = *pin_iterator;

    CHECK(netlist.name(pin_u1_o) == "u1:o");

    ++pin_iterator;

    auto pin_u1_b = *pin_iterator;

    CHECK(netlist.name(pin_u1_b) == "u1:b");

    ++pin_iterator;

    auto pin_u1_a = *pin_iterator;

    CHECK(netlist.name(pin_u1_a) == "u1:a");

    ++pin_iterator;

    CHECK(pin_iterator == cell_u1_pins.end());
}

TEST_CASE("Netlist factory: populate with simple verilog and simple standard cells.", "[circuit][Netlist][factory]")
{
    auto netlist = Netlist{};

    auto std_cells = StandardCells{};

    auto lef = ophidian::parser::Lef{"input_files/simple/simple.lef"};
    auto simple = ophidian::parser::Verilog{"input_files/simple/simple.v"};

    factory::make_standard_cells(std_cells, lef);
    factory::make_netlist(netlist, simple, std_cells);

    CHECK(netlist.size_cell_instance() == 6);

    CHECK(netlist.size_pin_instance() == 19);
    
    CHECK(netlist.size_net() == 9);

    CHECK(netlist.size_input_pad() == 3);

    CHECK(netlist.size_output_pad() == 1);

    auto cell_u1 = netlist.find_cell_instance("u1");

    CHECK(netlist.name(cell_u1) == "u1");

    auto std_cell_u1 = netlist.std_cell(cell_u1);

    CHECK(std_cells.name(std_cell_u1) == "NAND2_X1");

    auto cell_u1_pins = netlist.pins(cell_u1);

    CHECK(cell_u1_pins.size() == 3);

    auto pin_iterator = cell_u1_pins.begin();

    auto pin_u1_o = *pin_iterator;

    CHECK(netlist.name(pin_u1_o) == "u1:o");

    auto std_pin_u1_o = netlist.std_cell_pin(pin_u1_o);

    CHECK(std_cells.name(std_pin_u1_o) == "NAND2_X1:o");

    ++pin_iterator;

    auto pin_u1_b = *pin_iterator;

    CHECK(netlist.name(pin_u1_b) == "u1:b");

    auto std_pin_u1_b = netlist.std_cell_pin(pin_u1_b);

    CHECK(std_cells.name(std_pin_u1_b) == "NAND2_X1:b");

    ++pin_iterator;

    auto pin_u1_a = *pin_iterator;

    CHECK(netlist.name(pin_u1_a) == "u1:a");

    auto std_pin_u1_a = netlist.std_cell_pin(pin_u1_a);

    CHECK(std_cells.name(std_pin_u1_a) == "NAND2_X1:a");

    ++pin_iterator;

    CHECK(pin_iterator == cell_u1_pins.end());
}

TEST_CASE("Netlist factory: populate with ispd18 sample def and sample standard cells.", "[circuit][Netlist][factory]")
{
    auto netlist = Netlist{};

    auto std_cells = StandardCells{};

    auto lef = ophidian::parser::Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};
    auto def = ophidian::parser::Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};

    factory::make_standard_cells(std_cells, lef);
    factory::make_netlist(netlist, def, std_cells);

    CHECK(netlist.size_cell_instance() == 22);

    CHECK(netlist.size_pin_instance() == 22);

    CHECK(netlist.size_net() == 11);

    CHECK(netlist.size_input_pad() == 0);

    CHECK(netlist.size_output_pad() == 0);

    auto cell_u1 = netlist.find_cell_instance("inst2015");

    CHECK(netlist.name(cell_u1) == "inst2015");

    auto std_cell_u1 = netlist.std_cell(cell_u1);

    CHECK(std_cells.name(std_cell_u1) == "NAND3X2");

    auto cell_u1_pins = netlist.pins(cell_u1);

    CHECK(cell_u1_pins.size() == 1);

    auto pin_iterator = cell_u1_pins.begin();

    auto pin_u1_o = *pin_iterator;

    CHECK(netlist.name(pin_u1_o) == "inst2015:Y");

    auto std_pin_u1_o = netlist.std_cell_pin(pin_u1_o);

    CHECK(std_cells.name(std_pin_u1_o) == "NAND3X2:Y");

    auto net_pin_u1_o = netlist.net(pin_u1_o);

    CHECK(netlist.name(net_pin_u1_o) == "net1240");
}


TEST_CASE("Make a netlist from an iccad2020 file.", "[circuit][Netlist][factory]")
{
    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/case1.txt"};
    auto std_cells = ophidian::circuit::StandardCells{};
    auto netlist = Netlist{};
    ophidian::circuit::factory::make_standard_cells(std_cells, sample);
    ophidian::circuit::factory::make_netlist(netlist, sample, std_cells);

    SECTION("Check container sizes"){
        REQUIRE(netlist.size_cell_instance() == 8);
        REQUIRE(netlist.size_pin_instance() == 17);
        REQUIRE(netlist.size_net() == 6);
    }
    SECTION("Check if elements correctly connected"){
        auto inst_C2 = netlist.find_cell_instance("C2");
        REQUIRE(netlist.pins(inst_C2).size() == 3);
        auto std_cell = netlist.std_cell(inst_C2);
        REQUIRE(std_cells.name(std_cell) == "MC3");
        std::vector<std::string> std_pin_names,
                                 pin_names,
                                 expected_std_pin_names{"MC3:P1",
                                                        "MC3:P2",
                                                        "MC3:P3"},
                                 expected_pin_names{"C2:P1",
                                                    "C2:P2",
                                                    "C2:P3"};
        for(auto pin : netlist.pins(inst_C2))
        {
            auto std_pin = netlist.std_cell_pin(pin);
            pin_names.push_back(netlist.name(pin));
            std_pin_names.push_back(std_cells.name(std_pin));
        }
        REQUIRE(std::is_permutation(std_pin_names.begin(),
                                    std_pin_names.end(),
                                    expected_std_pin_names.begin()));

        REQUIRE(std::is_permutation(pin_names.begin(),
                                    pin_names.end(),
                                    expected_pin_names.begin()));

        auto net_N2 = netlist.find_net("N2");
        REQUIRE(netlist.pins(net_N2).size() == 4);
        std::vector<std::string> net_pins,
                                 expected_net_pins{"C2:P2",
                                                   "C5:P1",
                                                   "C8:P2",
                                                   "C7:P1"};
        for(auto pin : netlist.pins(net_N2))
            net_pins.push_back(netlist.name(pin));
        REQUIRE(std::is_permutation(net_pins.begin(),
                                    net_pins.end(),
                                    expected_net_pins.begin()));
    }
}
