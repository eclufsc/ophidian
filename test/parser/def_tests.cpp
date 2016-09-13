#include <catch.hpp>

#include <ophidian/parser/Def.h>

TEST_CASE("Def: Loading simple.def", "[parser][Def]")
{
    ophidian::parser::Def parser("input_files/simple.def");
    
    SECTION("Def: Diearea lower and upper points"){
        REQUIRE(parser.die().lower.x == 0);
        REQUIRE(parser.die().lower.y == 0);
        REQUIRE(parser.die().upper.x == 27360);
        REQUIRE(parser.die().upper.y == 13680);
    }
    SECTION("Def: Checking Components vector"){
        auto components = parser.components();
        REQUIRE(components.size() == 6);
        REQUIRE(components[0].name == "u1");
        REQUIRE(components[0].macro == "NAND2_X1");
        REQUIRE(components[0].orientation == "N");
        REQUIRE(components[0].position.x == 3420);
        REQUIRE(components[0].position.y == 6840);
        REQUIRE(!components[0].fixed);
    }
}

