#include "design_test.h"
#include <catch.hpp>

#include <ophidian/design/DesignBuilder.h>

using namespace ophidian::designBuilder;

TEST_CASE("DesignBuilder: building a 2017 design.", "[design]")
{
	DesignBuilder ICCAD2017DesignBuilder;
	ICCAD2017DesignBuilder.build2017("/home/mickeal-carboculus/Downloads/sample/pci_bridge32_a_md1/cells_modified.lef",
								 "/home/mickeal-carboculus/Downloads/sample/pci_bridge32_a_md1/tech.lef",
								 "/home/mickeal-carboculus/Downloads/sample/pci_bridge32_a_md1/placed.def");

	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Cell()));
	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Pin()));
	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Net()));
}


TEST_CASE("DesignBuilder: building a 2015 design.", "[design]")
{
	DesignBuilder ICCAD2017DesignBuilder;
	ICCAD2017DesignBuilder.build2015("/home/mickeal-carboculus/Downloads/superblue18/superblue18.lef",
								 "/home/mickeal-carboculus/Downloads/superblue18/superblue18.def",
								 "/home/mickeal-carboculus/Downloads/superblue18/superblue18.v");

	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Cell()));
	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Pin()));
	printf("%u\n", ICCAD2017DesignBuilder.design().netlist().size(ophidian::circuit::Net()));
}
