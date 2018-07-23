#include <catch.hpp>

#include <boost/geometry.hpp>

#include <ophidian/placement/Placement.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

class NetlistFixture {
public:
    CellInstance cell1, cell2;

    PinInstance pin1, pin2, pin3, pin4;

    Input input1, input2;

    Output output1, output2;

    Netlist netlist;

    NetlistFixture() {
        cell1 = netlist.add_cell_instance("cell1");
        cell2 = netlist.add_cell_instance("cell2");

        pin1 = netlist.add_pin_instance("pin1");
        pin2 = netlist.add_pin_instance("pin2");
        pin3 = netlist.add_pin_instance("pin3");
        pin4 = netlist.add_pin_instance("pin4");

        input1 = netlist.add_input_pad(pin1);
        input2 = netlist.add_input_pad(pin2);

        output1 = netlist.add_output_pad(pin3);
        output2 = netlist.add_output_pad(pin4);
    }
};

// TEST_CASE_METHOD(NetlistFixture, "Placement: placing a cell", "[placement]") {
//     auto placement = Placement{netlist};
//
//     auto cell1Location = ophidian::util::LocationDbu(10, 20);
//     placement.placeCell(cell1, cell1Location);
//
//     ophidian::util::LocationDbu cell2Location(20, 10);
//     placement.placeCell(cell2, cell2Location);
//
//     ophidian::util::LocationDbu placedCell1Location = placement.cellLocation(cell1);
//     ophidian::util::LocationDbu placedCell2Location = placement.cellLocation(cell2);
//     REQUIRE(cell1Location == placedCell1Location);
//     REQUIRE(cell2Location == placedCell2Location);
//     REQUIRE(!(placedCell1Location == placedCell2Location));
// }
//
// TEST_CASE_METHOD(NetlistFixture, "Placement: placing an input pad", "[placement]") {
//     Placement placement(netlist);
//
//     ophidian::util::LocationDbu input1Location(10, 20);
//     placement.placeInputPad(input1, input1Location);
//
//     ophidian::util::LocationDbu input2Location(20, 10);
//     placement.placeInputPad(input2, input2Location);
//
//     REQUIRE(placement.inputPadLocation(input1) == input1Location);
//     REQUIRE(placement.inputPadLocation(input2) == input2Location);
//     REQUIRE(placement.inputPadLocation(input1) != placement.inputPadLocation(input2));
// }
//
// TEST_CASE_METHOD(NetlistFixture, "Placement: placing an output pad", "[placement]") {
//     Placement placement(netlist);
//
//     ophidian::util::LocationDbu output1Location(10, 20);
//     placement.placeOutputPad(output1, output1Location);
//
//     ophidian::util::LocationDbu output2Location(20, 10);
//     placement.placeOutputPad(output2, output2Location);
//
//     REQUIRE(placement.outputPadLocation(output1) == output1Location);
//     REQUIRE(placement.outputPadLocation(output2) == output2Location);
//     REQUIRE(placement.outputPadLocation(output1) != placement.outputPadLocation(output2));
// }
