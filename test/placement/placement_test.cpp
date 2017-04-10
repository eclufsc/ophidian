#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Placement.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

namespace {
class NetlistFixture {
public:
    Cell cell1, cell2;

    Pin pin1, pin2, pin3, pin4;

    Input input1, input2;

    Output output1, output2;

    Netlist netlist;

    NetlistFixture() {
        cell1 = netlist.add(Cell());
        cell2 = netlist.add(Cell());

        pin1 = netlist.add(Pin());
        pin2 = netlist.add(Pin());
        pin3 = netlist.add(Pin());
        pin4 = netlist.add(Pin());

        input1 = netlist.add(Input(), pin1);
        input2 = netlist.add(Input(), pin2);

        output1 = netlist.add(Output(), pin3);
        output2 = netlist.add(Output(), pin4);
    }
};
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing a cell", "[placement]") {
    Placement placement(netlist);

    ophidian::util::Location cell1Location(10, 20);
    placement.placeCell(cell1, cell1Location);

    ophidian::util::Location cell2Location(20, 10);
    placement.placeCell(cell2, cell2Location);

    ophidian::util::Location placedCell1Location = placement.cellLocation(cell1);
    ophidian::util::Location placedCell2Location = placement.cellLocation(cell2);
    REQUIRE(cell1Location == placedCell1Location);
    REQUIRE(cell2Location == placedCell2Location);
    REQUIRE(!(placedCell1Location == placedCell2Location));
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing an input pad", "[placement]") {
    Placement placement(netlist);

    ophidian::util::Location input1Location(10, 20);
    placement.placeInputPad(input1, input1Location);

    ophidian::util::Location input2Location(20, 10);
    placement.placeInputPad(input2, input2Location);

    REQUIRE(placement.inputPadLocation(input1) == input1Location);
    REQUIRE(placement.inputPadLocation(input2) == input2Location);
    REQUIRE(placement.inputPadLocation(input1) != placement.inputPadLocation(input2));
}

TEST_CASE_METHOD(NetlistFixture, "Placement: placing an output pad", "[placement]") {
    Placement placement(netlist);

    ophidian::util::Location output1Location(10, 20);
    placement.placeOutputPad(output1, output1Location);

    ophidian::util::Location output2Location(20, 10);
    placement.placeOutputPad(output2, output2Location);

    REQUIRE(placement.outputPadLocation(output1) == output1Location);
    REQUIRE(placement.outputPadLocation(output2) == output2Location);
    REQUIRE(placement.outputPadLocation(output1) != placement.outputPadLocation(output2));
}
