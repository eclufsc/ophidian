#include <catch.hpp>

#include <ophidian/parser/Def.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/ParserException.h>

using ophidian::parser::Lef;
using ophidian::parser::Def;
using micron_t = ophidian::parser::Lef::micrometer_type;

TEST_CASE("lef: missing file", "[parser][lef][missing_file]")
{
    CHECK_THROWS_AS(
        Lef{"thisFileDoesNotExist.lef"},
        ophidian::parser::exceptions::InexistentFile
    );
} 

TEST_CASE("lef: simple.lef parsing", "[parser][lef][simple]")
{
    auto simple = Lef{"input_files/simple/simple.lef"};

    SECTION("Database units are correct", "[parser][lef][simple][ratio]")
    {
        CHECK(simple.micrometer_to_dbu_ratio() == 2000.0);

        auto x = Lef::micrometer_type{1.5};

        auto microns = x * simple.micrometer_to_dbu_ratio();

        CHECK(microns == Lef::micrometer_type{3000});

        auto converted_to_dbu = Def::database_unit_type{3000};

        CHECK(converted_to_dbu == Def::database_unit_type{x*simple.micrometer_to_dbu_ratio()});
    }

    SECTION("Sites are parsed correctly", "[parser][lef][simple][sites]")
    {
        CHECK( simple.sites().size() == 1 );

        auto core = simple.sites().front();
        CHECK(core.name() == "core");
        CHECK(core.class_name() == "CORE");
        CHECK(core.width() == micron_t{0.19});
        CHECK(core.height() == micron_t{1.71});
    }

    SECTION("Layers are parsed correctly", "[parser][lef][simple][layers]")
    {
        CHECK(simple.layers().size() == 3);

        auto& first_layer = simple.layers().front();
        CHECK(first_layer.name() == "metal1");
        CHECK(first_layer.type() == Lef::layer_type::type_type::ROUTING);
        CHECK(first_layer.direction() == Lef::Layer::direction_type::HORIZONTAL);
        CHECK(first_layer.pitch() == micron_t{0.2});
        CHECK(first_layer.offset() == micron_t{0.0});
        CHECK(first_layer.width() == micron_t{0.1});
        
        auto& last_layer = simple.layers().back();
        CHECK(last_layer.name() == "metal3");
        CHECK(last_layer.type() == Lef::layer_type::type_type::ROUTING);
        CHECK(last_layer.direction() == Lef::layer_type::direction_type::HORIZONTAL);
        CHECK(last_layer.pitch() == micron_t{0.2});
        CHECK(last_layer.offset() == micron_t{0.0});
        CHECK(last_layer.width() == micron_t{0.1});
    }

    SECTION("Macros are parsed correctly", "[parser][lef][simple][macros]")
    {
        CHECK( simple.macros().size() == 212 );

        auto& first_macro = simple.macros().front();

        CHECK(first_macro.name() == "INV_X1");
        CHECK(first_macro.class_name() == "CORE");

        CHECK(first_macro.size().x() == micron_t{0.760});
        CHECK(first_macro.size().y() == micron_t{1.71});

        CHECK(first_macro.foreign().name == "INV_X1");
        CHECK(first_macro.foreign().offset.x() == micron_t{0.0});
        CHECK(first_macro.foreign().offset.y() == micron_t{0.0});

        CHECK(first_macro.origin().x() == micron_t{0.0});
        CHECK(first_macro.origin().y() == micron_t{0.0});

        CHECK(first_macro.site() == "core");

        CHECK(first_macro.pins().size() == 2);
        
        auto& first_pin_fm = first_macro.pins().front();

        CHECK(first_pin_fm.name() == "o");
        CHECK(first_pin_fm.direction() == Lef::macro_type::pin_type::direction_type::OUTPUT);

        auto rects = first_pin_fm.ports().at(simple.layers().front().name());

        CHECK(rects.front().min_corner().x() == micron_t{0.465});
        CHECK(rects.front().min_corner().y() == micron_t{0.150});
        CHECK(rects.front().max_corner().x() == micron_t{0.53});
        CHECK(rects.front().max_corner().y() == micron_t{1.255});

        CHECK(rects.back().min_corner().x() == micron_t{0.415});
        CHECK(rects.back().min_corner().y() == micron_t{0.150});
        CHECK(rects.back().max_corner().x() == micron_t{0.61});
        CHECK(rects.back().max_corner().y() == micron_t{0.28});

        CHECK(first_macro.obstructions().empty());
    }
}
