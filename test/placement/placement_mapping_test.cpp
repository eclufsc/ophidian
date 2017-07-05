#include <catch.hpp>

#include <ophidian/placement/PlacementMapping.h>

using namespace ophidian::geometry;
using namespace ophidian::util;

namespace {
class LibraryMappingFixture {
public:
    ophidian::circuit::Cell cell1, cell2;
    ophidian::circuit::Pin pin1, pin2;
    ophidian::standard_cell::Cell stdCell1, stdCell2;
    ophidian::standard_cell::Pin stdCell3, stdCell4;

    ophidian::circuit::Netlist netlist;
    ophidian::standard_cell::StandardCells stdCells;

    ophidian::circuit::LibraryMapping libraryMapping;

    LibraryMappingFixture()
        : libraryMapping(netlist) {
        cell1 = netlist.add(ophidian::circuit::Cell(), "cell1");
        cell2 = netlist.add(ophidian::circuit::Cell(), "cell2");

        pin1 = netlist.add(ophidian::circuit::Pin(), "pin1");
        pin2 = netlist.add(ophidian::circuit::Pin(), "pin2");

        netlist.add(cell1, pin1);
        netlist.add(cell2, pin2);

        stdCell1 = stdCells.add(ophidian::standard_cell::Cell(), "INVX1");
        stdCell2 = stdCells.add(ophidian::standard_cell::Cell(), "INVX2");
        stdCell3 = stdCells.add(ophidian::standard_cell::Pin(), "INVX1_in", ophidian::standard_cell::PinDirection::INPUT);
        stdCell4 = stdCells.add(ophidian::standard_cell::Pin(), "INVX2_out", ophidian::standard_cell::PinDirection::OUTPUT);

        libraryMapping.cellStdCell(cell1, stdCell1);
        libraryMapping.cellStdCell(cell2, stdCell2);
        libraryMapping.pinStdCell(pin1, stdCell3);
        libraryMapping.pinStdCell(pin2, stdCell4);
    }
};

class PlacementAndLibraryFixture {
public:

    const LibraryMappingFixture & libraryMappingFixture;

    ophidian::placement::Placement placement;
    ophidian::placement::Library library;

    PlacementAndLibraryFixture(const LibraryMappingFixture & libraryMappingFixture)
        : placement(libraryMappingFixture.netlist), library(libraryMappingFixture.stdCells), libraryMappingFixture(libraryMappingFixture) {

        placement.placeCell(libraryMappingFixture.cell1, ophidian::util::LocationDbu(5, 10));
        placement.placeCell(libraryMappingFixture.cell2, ophidian::util::LocationDbu(35, 32));

        std::vector<Box> cell1Boxes = {Box(Point(0, 0), Point(10, 10))};
        MultiBox cell1Geometry(cell1Boxes);
        library.geometry(libraryMappingFixture.stdCell1, cell1Geometry);

        std::vector<Box> cell2Boxes = {Box(Point(0, 0), Point(20, 10))};
        MultiBox cell2Geometry(cell2Boxes);
        library.geometry(libraryMappingFixture.stdCell2, cell2Geometry);

        library.pinOffset(libraryMappingFixture.stdCell3, ophidian::util::LocationDbu(2, 3));
        library.pinOffset(libraryMappingFixture.stdCell4, ophidian::util::LocationDbu(1, 4));
    }
};
}

TEST_CASE("Placement Mapping: getting cell geometry", "[placement_mapping][cell_geometry]") {
    LibraryMappingFixture libraryMappingFixture;
    PlacementAndLibraryFixture placementAndLibraryFixture(libraryMappingFixture);

    ophidian::placement::PlacementMapping placementMapping(placementAndLibraryFixture.placement, placementAndLibraryFixture.library,
                                                           libraryMappingFixture.netlist, libraryMappingFixture.libraryMapping);

    std::vector<Box> cell1ExpectedBoxes = {Box(Point(5, 10), Point(15, 20))};
    MultiBox cell1ExpectedGeometry(cell1ExpectedBoxes);

    std::vector<Box> cell2ExpectedBoxes = {Box(Point(35, 32), Point(55, 42))};
    MultiBox cell2ExpectedGeometry(cell2ExpectedBoxes);

    REQUIRE(placementMapping.geometry(libraryMappingFixture.cell1) == cell1ExpectedBoxes);
    REQUIRE(placementMapping.geometry(libraryMappingFixture.cell2) == cell2ExpectedBoxes);
    REQUIRE(placementMapping.geometry(libraryMappingFixture.cell1) != placementMapping.geometry(libraryMappingFixture.cell2));
}

TEST_CASE("Placement Mapping: getting pin location", "[placement_mapping][pin_location]") {
    LibraryMappingFixture libraryMappingFixture;
    PlacementAndLibraryFixture placementAndLibraryFixture(libraryMappingFixture);

    ophidian::placement::PlacementMapping placementMapping(placementAndLibraryFixture.placement, placementAndLibraryFixture.library,
                                                           libraryMappingFixture.netlist, libraryMappingFixture.libraryMapping);

    LocationDbu pin1ExpectedLocation(7, 13);
    LocationDbu pin2ExpectedLocation(36, 36);

    REQUIRE(placementMapping.location(libraryMappingFixture.pin1) == pin1ExpectedLocation);
    REQUIRE(placementMapping.location(libraryMappingFixture.pin2) == pin2ExpectedLocation);
    REQUIRE(placementMapping.location(libraryMappingFixture.pin1) != placementMapping.location(libraryMappingFixture.pin2));
}
