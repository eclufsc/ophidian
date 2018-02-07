  #include "liberty_test.h"
#include <catch.hpp>

#include <ophidian/parser/LibertyParser.h>
#include <istream>
#include <fstream>
#include <iostream>

#include <memory>
#include <vector>

using namespace ophidian::parser;

TEST_CASE("Liberty: init", "[parser][LibertyParser]")
{
    LibertyParser libertyParser;
    std::shared_ptr<Liberty> lib = libertyParser.readFile("./input_files/sample2_Late.lib");
    auto liberty = lib.get();

    // Liberty: Liberty information
    REQUIRE(liberty->capacitiveLoadUnitValue == 1.0);
    REQUIRE(liberty->capacitiveLoadUnit.compare("ff") == 0);
    REQUIRE(liberty->timeUnit.compare("1ps") == 0);
    REQUIRE(liberty->cells.size() == 2);

    // Liberty: Cell information
    Liberty::Cell inverter = liberty->cells.front();
    REQUIRE(inverter.name.compare("INV_X1") == 0);
    REQUIRE(!inverter.sequential);
    REQUIRE(inverter.pins.size() == 2);

    Liberty::Cell flipflop = liberty->cells.back();
    REQUIRE(flipflop.name.compare("DFF_X80") == 0);
    REQUIRE(flipflop.sequential);
    REQUIRE(flipflop.pins.size() == 3);

    // Liberty: Pin information
    for(Liberty::Pin pin : inverter.pins)
    {
        if(pin.name == "o") {
            REQUIRE(pin.capacitance == 0);
            REQUIRE(pin.maxCapacitance == 12.8);
            REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::OUTPUT);
            REQUIRE(pin.timing.size() == 1);
        } else if(pin.name == "a") {
            REQUIRE(pin.capacitance == 1);
            REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::INPUT);
            REQUIRE(pin.timing.size() == 0);
        }
    }

    for(Liberty::Pin pin : flipflop.pins)
    {
        if(pin.name == "q") {
            REQUIRE(pin.capacitance == 0);
            REQUIRE(pin.maxCapacitance == 2048.00);
            REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::OUTPUT);
            REQUIRE(pin.timing.size() == 1);
        } else if(pin.name == "ck") {
            REQUIRE(pin.capacitance == 1.5);
            REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::INPUT);
            REQUIRE(pin.timing.size() == 0);
        } else if(pin.name == "d") {
            REQUIRE(pin.capacitance == 3.49);
            REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::INPUT);
            REQUIRE(pin.timing.size() == 2);
        }
    }

    // Liberty: Timing information
    Liberty::Timing inverterTimingOut = inverter.pins.front().timing.front();
    REQUIRE(inverterTimingOut.timingSense == Liberty::Timing::NEGATIVE_UNATE);
    REQUIRE(inverterTimingOut.timingType == Liberty::Timing::COMBINATIONAL);
    REQUIRE(inverterTimingOut.relatedPin == "a");
    REQUIRE(inverterTimingOut.luts.size() == 4);

    Liberty::Timing flipflopTimingQ = flipflop.pins.front().timing.front();
    REQUIRE(flipflopTimingQ.timingSense == Liberty::Timing::NON_UNATE);
    REQUIRE(flipflopTimingQ.timingType == Liberty::Timing::RISING_EDGE);
    REQUIRE(flipflopTimingQ.relatedPin == "ck");
    REQUIRE(flipflopTimingQ.luts.size() == 4);

    // Liberty: LUT information
    Liberty::LUT lutInverterOut = inverterTimingOut.luts.front();
    REQUIRE(lutInverterOut._lutInformation == Liberty::LUT::CELL_FALL);
    REQUIRE(lutInverterOut.index_1.size() == 7);
    REQUIRE(lutInverterOut.index_2.size() == 8);
    REQUIRE(lutInverterOut.index_1[1] == 1.00);
    REQUIRE(lutInverterOut.index_2[1] == 30.00);
    REQUIRE(lutInverterOut.values.size() == 7);
    REQUIRE(lutInverterOut.values[0].size() == 8);
    REQUIRE(lutInverterOut.values[1][1] == 28.116);

    Liberty::LUT lutFlipFlopQ = flipflopTimingQ.luts.back();
    REQUIRE(lutFlipFlopQ._lutInformation == Liberty::LUT::RISE_TRANSITION);
    REQUIRE(lutFlipFlopQ.index_1.size() == 7);
    REQUIRE(lutFlipFlopQ.index_2.size() == 8);
    REQUIRE(lutFlipFlopQ.index_1[6] == 4096.00);
    REQUIRE(lutFlipFlopQ.index_2[7] == 500.00);
    REQUIRE(lutFlipFlopQ.values.size() == 7);
    REQUIRE(lutFlipFlopQ.values[0].size() == 8);
    REQUIRE(lutFlipFlopQ.values[6][7] == 211.2);

    REQUIRE(liberty->find(inverter.name).name == inverter.name);
    REQUIRE(inverter.find(inverter.pins.front().name).name == inverter.pins.front().name);
    REQUIRE(inverter.pins.front().find(inverterTimingOut.timingType).timingType == inverterTimingOut.timingType);
    REQUIRE(inverterTimingOut.find(lutInverterOut._lutInformation)._lutInformation == lutInverterOut._lutInformation);
}


