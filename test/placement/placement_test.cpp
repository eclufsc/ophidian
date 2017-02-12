#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Placement.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

namespace {
class NetlistFixture {
public:
    Cell cell1, cell2;

    Netlist netlist;

    NetlistFixture() {
        cell1 = netlist.add(Cell());
        cell2 = netlist.add(Cell());
    }
};
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing a cell", "[placement]") {
    Placement placement(netlist);

    ophidian::util::Location cell1Location(10, 20);
    placement.place_cell(cell1, cell1Location);

    ophidian::util::Location cell2Location(20, 10);
    placement.place_cell(cell2, cell2Location);

    ophidian::util::Location placedCell1Location = placement.location(cell1);
    ophidian::util::Location placedCell2Location = placement.location(cell2);
    REQUIRE(cell1Location == placedCell1Location);
    REQUIRE(cell2Location == placedCell2Location);
    REQUIRE(!(placedCell1Location == placedCell2Location));
}
