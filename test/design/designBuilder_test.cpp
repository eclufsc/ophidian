#include <catch.hpp>

#include <ophidian/design/DesignBuilder.h>

using namespace ophidian::design;

// TEST_CASE("DesignBuilder: building a 2017 design.", "[design]")
// {
//     ICCAD2017ContestDesignBuilder ICCAD2017DesignBuilder("./input_files/pci_bridge32_a_md1/cells_modified.lef",
//                                                          "./input_files/pci_bridge32_a_md1/tech.lef",
//                                                          "./input_files/pci_bridge32_a_md1/placed.def");
//     Design & design = ICCAD2017DesignBuilder.build();
//
//     REQUIRE(design.netlist().size(ophidian::circuit::Cell()) == 29521);
// }
//
//
// TEST_CASE("DesignBuilder: build simple.", "[design]")
// {
//     ICCAD2015ContestDesignBuilder ICCAD2015DesignBuilder("./input_files/simple/simple.lef",
//                                                          "./input_files/simple/simple.def",
//                                                          "./input_files/simple/simple.v");
//     Design & design = ICCAD2015DesignBuilder.build();
//     REQUIRE(design.netlist().size(ophidian::circuit::Cell()) == 6);
//     REQUIRE(design.netlist().size(ophidian::circuit::Pin()) == 19);
//     REQUIRE(design.netlist().size(ophidian::circuit::Net()) == 9);
//
// }
