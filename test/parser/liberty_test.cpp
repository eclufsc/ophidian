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

    REQUIRE(liberty->capacitiveLoadUnitValue == 1.0);
    REQUIRE(liberty->capacitiveLoadUnit.compare("ff") == 0);
    REQUIRE(liberty->timeUnit.compare("1ps") == 0);
    REQUIRE(liberty->cells.size() == 2);

    Liberty::Cell cell = liberty->cells.front();
    if(cell.name.compare("INV_X1") == 0){
        REQUIRE( ! cell.sequential);
        for(Liberty::Pin pin : cell.pins){
            if(pin.name == "o"){
                REQUIRE(pin.capacitance == 0);
                REQUIRE(pin.maxCapacitance == 12.8);
                REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::OUTPUT);
            }else if(pin.name == "a"){
                REQUIRE(pin.capacitance == 1);
                REQUIRE(pin.pinDirection == Liberty::Pin::directionPin::INPUT);
            }
        }
    }
}
