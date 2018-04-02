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

TEST_CASE("Def: Loading simple.def", "[parser][Def][simple]")
{
    auto simple = Def{"input_files/simple/simple.def"};

    SECTION("Def: Check units distance", "[parser][Def][simple][ratio]"){
        CHECK(simple.dbu_to_micrometer_ratio() == 2000.0);
    }

    SECTION("Def: Diearea lower and upper points", "[parser][Def][simple][die_area]"){
        CHECK(simple.die_area().min_corner().x() == Def::dbu_type{0});
        CHECK(simple.die_area().min_corner().y() == Def::dbu_type{0});
        CHECK(simple.die_area().max_corner().x() == Def::dbu_type{27360});
        CHECK(simple.die_area().max_corner().y() == Def::dbu_type{13680});
    }

    SECTION("Def: Checking Component vector", "[parser][Def][simple][components]"){
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

    SECTION("Def: Checking Row vector", "[parser][Def][simple][rows]"){
        auto rows = simple.rows();
        CHECK(rows.size() == 4);

        CHECK(rows[0].name == "core_SITE_ROW_0");
        CHECK(rows[0].site == "core");
        CHECK(rows[0].origin.x() == Def::dbu_type{0.0});
        CHECK(rows[0].origin.y() == Def::dbu_type{0.0});
        CHECK(rows[0].step.x() == Def::dbu_type{380.0});
        CHECK(rows[0].step.y() == Def::dbu_type{0.0});
        CHECK(rows[0].num.x() == Def::scalar_type{72.0});
        CHECK(rows[0].num.y() == Def::scalar_type{1.0});

        CHECK(rows[3].name == "core_SITE_ROW_3");
        CHECK(rows[3].site == "core");
        CHECK(rows[3].origin.x() == Def::dbu_type{0.0});
        CHECK(rows[3].origin.y() == Def::dbu_type{10260.0});
        CHECK(rows[3].step.x() == Def::dbu_type{380.0});
        CHECK(rows[3].step.y() == Def::dbu_type{0.0});
        CHECK(rows[3].num.x() == Def::scalar_type{72.0});
        CHECK(rows[3].num.y() == Def::scalar_type{1.0});
    }
}
