#include "LookupTableTest.h"
#include <catch.hpp>

#include <ophidian/timing/LookupTable.h>

using namespace ophidian::timing;

TEST_CASE("timing look up table: create an look up table, and do a interpolation.", "[timing]") {
    LookupTable<float, 8> lut;
        lut.row = {1.0, 4.0, 16.0, 64.0, 256.0, 1024.0, 4096.0, 16384.0};
        lut.column = { 0.0, 2.0, 5.0, 20.0, 50.0, 80.0, 300.0, 500.0 };
        lut.values = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
                       8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0,
                       24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0};
        REQUIRE(lut.value(10.4, 22.0) == Approx(15.3333));
}

TEST_CASE("timing look up table: extrapolation on right and left.", "[timing]") {
    LookupTable<float, 8> lut;
        lut.row = {1.0, 4.0, 16.0, 64.0, 256.0, 1024.0, 4096.0, 16384.0};
        lut.column = { 1.0, 2.0, 5.0, 20.0, 50.0, 80.0, 300.0, 500.0 };
        lut.values = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
                       8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0,
                       24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0};
        REQUIRE(lut.value(10.4, 501.0) == Approx(19.2717));
        REQUIRE(lut.value(10.4, 0.5) == Approx(12.7667));//I'm not sure about this test
}

TEST_CASE("timing look up table: extrapolation on table top.", "[timing]") {
    LookupTable<float, 8> lut;
        lut.row = {1.0, 4.0, 16.0, 64.0, 256.0, 1024.0, 4096.0, 16384.0};
        lut.column = { 1.0, 2.0, 5.0, 20.0, 50.0, 80.0, 300.0, 500.0 };
        lut.values = { 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
                       8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0,
                       24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0};
        REQUIRE(lut.value(0.5, 0.5) == Approx(2.04167));//I'm not sure about this test
}

TEST_CASE("timing look up table: interpolation when cv value is zero or below.", "[timing]") {
    LookupTable<float, 8> lut;
        lut.row = {1.0, 4.0, 16.0, 64.0, 256.0, 1024.0, 4096.0, 16384.0};
        lut.column = { 0.0, 2.0, 5.0, 20.0, 50.0, 80.0, 300.0, 500.0 };
        lut.values = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
                       8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0,
                       24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0};
        REQUIRE(lut.value(10.4, 0.0) == Approx(0));
        REQUIRE(lut.value(10.4, -0.1) == Approx(0));
}
