#include <catch.hpp>

#include <ophidian/parser/LibertyParser.h>

using namespace ophidian::parser;

TEST_CASE("Liberty: init", "[parser][LibertyParser]")
{
    LibertyParser libertyParser;
    std::shared_ptr<Liberty> lib = libertyParser.readFile("./input_files/tau2015/simple/simple_Early.lib");
    auto liberty = lib.get();

    CHECK(liberty->capacitiveLoadUnitValue == 1.0);
    CHECK(liberty->capacitiveLoadUnit.compare("ff") == 0);
    CHECK(liberty->timeUnit.compare("1ps") == 0);
    CHECK(liberty->cells.size() == 211);

    Liberty::Cell cell = liberty->cells.front();
    if(cell.name.compare("INV_X1") == 0){
        CHECK( ! cell.sequential);
        for(Liberty::Pin pin : cell.pins){
            if(pin.name == "o"){
                CHECK(pin.capacitance == 0);
                CHECK(pin.maxCapacitance == 12.8);
                CHECK(pin.pinDirection == Liberty::Pin::directionPin::OUTPUT);
            }else if(pin.name == "a"){
                CHECK(pin.capacitance == 1);
                CHECK(pin.pinDirection == Liberty::Pin::directionPin::INPUT);
            }
        }
    }
}
