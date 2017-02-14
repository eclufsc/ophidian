#include "LookupTableTest.h"
#include <catch.hpp>

#include <ophidian/timing/LookupTable.h>

using namespace ophidian::timing;

TEST_CASE("timing look up table: create an look up table.", "[timing]") {
    LookupTable<float, 3> lut;
    lut.row = {1.0, 2.0, 3.0};
    lut.column = {1.0, 2.0, 3.0};
    lut.values = { 3.0, 4.0, 5.0,
                   5.0, 6.0, 7.0,
                   7.0, 8.0, 9.0};
    REQUIRE(lut.row.size() == 3);
    REQUIRE(lut.column.size() == 3);
    REQUIRE(lut.values.size() == 3*3);
    REQUIRE(lut.values.at(lut.index(0, 0)) == 3.0);
    REQUIRE(lut.values.at(lut.index(2, 2)) == 9.0);
}

TEST_CASE("timing look up table: interpolation when cv value is zero or below.", "[timing]") {
    LookupTable<float, 3> lut;
    lut.row = {1.0, 2.0, 3.0};
    lut.column = {1.0, 2.0, 3.0};
    lut.values = { 3.0, 4.0, 5.0,
                   5.0, 6.0, 7.0,
                   7.0, 8.0, 9.0};
    REQUIRE(lut.value(1.0, -0.1) == Approx(0));
    REQUIRE(lut.value(1.0, 0.0) == Approx(2.0));
}

TEST_CASE("timing look up table: test some interpolations.", "[timing]") {
    LookupTable<float, 3> lut;
    lut.row = {1.0, 2.0, 3.0};
    lut.column = {1.0, 2.0, 3.0};
    lut.values = { 3.0, 4.0, 5.0,
                   5.0, 6.0, 7.0,
                   7.0, 8.0, 9.0};
    REQUIRE(lut.value(2.0, 2.0) == 6.0);
    REQUIRE(lut.value(1.0, 2.5) == 4.5);
    REQUIRE(lut.value(2.5, 1.0) == 6.0);
    REQUIRE(lut.value(1.0, 1.5) == 3.5);
}

TEST_CASE("timing look up table: test some extrapolations.", "[timing]") {
    LookupTable<float, 3> lut;
    lut.row = {1.0, 2.0, 3.0};
    lut.column = {1.0, 2.0, 3.0};
    lut.values = { 3.0, 4.0, 5.0,
                   5.0, 6.0, 7.0,
                   7.0, 8.0, 9.0};
    //top left corner
    REQUIRE(lut.value(0.5, 1.0) == 2.0);
    REQUIRE(lut.value(1.0, 0.5) == 2.5);
    REQUIRE(lut.value(0.5, 0.5) == 1.5);

    //top right corner
    REQUIRE(lut.value(1.0, 4.0) == 6.0);
    REQUIRE(lut.value(0.5, 3.0) == 4.0);
    REQUIRE(lut.value(0.5, 4.0) == 5.0);

    //botton right corner
    REQUIRE(lut.value(4.0, 3.0) == 11.0);
    REQUIRE(lut.value(4.0, 4.0) == 12.0);

    //botton left corner
    REQUIRE(lut.value(4.0, 0.5) == 8.5);
}

TEST_CASE("timing look up table: when table have duplicated values for a row or column.", "[timing]") {
    //TODO
}
