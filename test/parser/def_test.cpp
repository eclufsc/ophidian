#include <catch.hpp>

#include <ophidian/parser/Def.h>
#include <ophidian/parser/ParserException.h>

using ophidian::parser::Def;

TEST_CASE("Def: Try to load inexistent file", "[parser][Def]")
{
    CHECK_THROWS_AS(
        Def{"a_file_with_this_name_should_not_exist"},
        ophidian::parser::InexistentFile
    );
}

TEST_CASE("Def: Loading simple.def", "[parser][Def]")
{
    auto simple = Def{"./input_files/simple/simple.def"};

    SECTION("Def: Diearea lower and upper points"){
        CHECK(simple.die_area().min_corner().x() == Def::dbu_type{0});
        CHECK(simple.die_area().min_corner().y() == Def::dbu_type{0});
        CHECK(simple.die_area().max_corner().x() == Def::dbu_type{27360});
        CHECK(simple.die_area().max_corner().y() == Def::dbu_type{13680});
    }
    SECTION("Def: Checking Component vector"){
        auto components = simple.components();
        CHECK(components.size() == 6);

        CHECK(components[0].name == "u1");
        CHECK(components[0].macro == "NAND2_X1");
        CHECK(components[0].orientation == Def::Component::Orientation::N);
        CHECK(components[0].position.x() == Def::dbu_type{3420});
        CHECK(components[0].position.y() == Def::dbu_type{6840});
        CHECK_FALSE(components[0].fixed);

        CHECK(components[2].name == "f1");
        CHECK(components[2].macro == "DFF_X80");
        CHECK(components[2].orientation == Def::Component::Orientation::N);
        CHECK(components[2].position.x() == Def::dbu_type{760});
        CHECK(components[2].position.y() == Def::dbu_type{0});
        CHECK(components[2].fixed);
    }
    // SECTION("Def: Checking Row vector"){
    //     auto rows = simple.rows();
    //     CHECK(rows.size() == 4);
    //
    //     CHECK(rows[0].name == "core_SITE_ROW_0");
    //     CHECK(rows[0].site == "core");
    //     CHECK(rows[0].origin.x == 0.0);
    //     CHECK(rows[0].origin.y == 0.0);
    //     CHECK(rows[0].step.x == 380.0);
    //     CHECK(rows[0].step.y == 0.0);
    //     CHECK(rows[0].num.x == 72.0);
    //     CHECK(rows[0].num.y == 1.0);
    //
    //     CHECK(rows[3].name == "core_SITE_ROW_3");
    //     CHECK(rows[3].site == "core");
    //     CHECK(rows[3].origin.x == 0.0);
    //     CHECK(rows[3].origin.y == 10260.0);
    //     CHECK(rows[3].step.x == 380.0);
    //     CHECK(rows[3].step.y == 0.0);
    //     CHECK(rows[3].num.x == 72.0);
    //     CHECK(rows[3].num.y == 1.0);
    // }
    // SECTION("Def: Check units distance"){
    //     CHECK(simple.database_units() == 2000.0);
    // }
}
