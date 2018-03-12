#include <catch.hpp>

#include <ophidian/parser/Def.h>
#include <ophidian/parser/ParserException.h>

TEST_CASE("Def: Try to load inexistent file", "[parser][Def]")
{
    ophidian::parser::DefParser reader;
    REQUIRE_THROWS_AS(reader.readFile("a_file_with_this_name_should_not_exist"), 
            ophidian::parser::InexistentFile);
}

TEST_CASE("Def: Loading simple.def", "[parser][Def]")
{
    ophidian::parser::DefParser reader;
    std::shared_ptr<ophidian::parser::Def> parser = reader.readFile("./input_files/simple/simple.def");
    
    SECTION("Def: Diearea lower and upper points"){
        CHECK(parser->die().lower.x == 0);
        CHECK(parser->die().lower.y == 0);
        CHECK(parser->die().upper.x == 27360);
        CHECK(parser->die().upper.y == 13680);
    }
    SECTION("Def: Checking Component vector"){
        auto components = parser->components();
        CHECK(components.size() == 6);
        
        CHECK(components[0].name == "u1");
        CHECK(components[0].macro == "NAND2_X1");
        CHECK(components[0].orientation == "N");
        CHECK(components[0].position.x == 3420);
        CHECK(components[0].position.y == 6840);
        CHECK_FALSE(components[0].fixed);
        
        CHECK(components[2].name == "f1");
        CHECK(components[2].macro == "DFF_X80");
        CHECK(components[2].orientation == "N");
        CHECK(components[2].position.x == 760);
        CHECK(components[2].position.y == 0);
        CHECK(components[2].fixed);
    }
    SECTION("Def: Checking Row vector"){
        auto rows = parser->rows();
        CHECK(rows.size() == 4);

        CHECK(rows[0].name == "core_SITE_ROW_0");
        CHECK(rows[0].site == "core");
        CHECK(rows[0].origin.x == 0.0);
        CHECK(rows[0].origin.y == 0.0);
        CHECK(rows[0].step.x == 380.0);
        CHECK(rows[0].step.y == 0.0);
        CHECK(rows[0].num.x == 72.0);
        CHECK(rows[0].num.y == 1.0);
        
        CHECK(rows[3].name == "core_SITE_ROW_3");
        CHECK(rows[3].site == "core");
        CHECK(rows[3].origin.x == 0.0);
        CHECK(rows[3].origin.y == 10260.0);
        CHECK(rows[3].step.x == 380.0);
        CHECK(rows[3].step.y == 0.0);
        CHECK(rows[3].num.x == 72.0);
        CHECK(rows[3].num.y == 1.0);
    }
    SECTION("Def: Check units distance"){
        CHECK(parser->database_units() == 2000.0);
    }
}
