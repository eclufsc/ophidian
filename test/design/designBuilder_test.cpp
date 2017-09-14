#include "design_test.h"
#include <catch.hpp>

#include <ophidian/design/DesignBuilder.h>

using namespace ophidian::design;

TEST_CASE("DesignBuilder: building a 2017 design.", "[design]")
{
	ICCAD2017ContestDesignBuilder ICCAD2017DesignBuilder("./input_files/pci_bridge32_a_md1/cells_modified.lef",
														 "./input_files/pci_bridge32_a_md1/tech.lef",
														 "./input_files/pci_bridge32_a_md1/placed.def");
    Design & design = ICCAD2017DesignBuilder.build();

    REQUIRE(design.netlist().size(ophidian::circuit::Cell()) == 29521);
}


TEST_CASE("DesignBuilder: building a 2015 design.", "[design]")
{
	ICCAD2015ContestDesignBuilder ICCAD2015DesignBuilder("./input_files/superblue18/superblue18.lef",
														 "./input_files/superblue18/superblue18.def",
														 "./input_files/superblue18/superblue18.v");
    Design & design = ICCAD2015DesignBuilder.build();
    REQUIRE(design.netlist().size(ophidian::circuit::Cell()) == 768068);
    REQUIRE(design.netlist().size(ophidian::circuit::Pin()) == 2559143);
    REQUIRE(design.netlist().size(ophidian::circuit::Net()) == 771542);

}
