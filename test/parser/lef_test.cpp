#include <catch.hpp>
#include <algorithm>

#include <ophidian/parser/Def.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/ParserException.h>

using ophidian::parser::Lef;
using ophidian::parser::Def;
using micron_t = ophidian::parser::Lef::micrometer_type;

bool boxComparator(const Lef::via_type::layer_container_type::value_type& a, const Lef::via_type::layer_container_type::value_type& b)
{
    return a.min_corner().x() == b.min_corner().x() && a.max_corner().x() == b.max_corner().x() && a.min_corner().y() == b.min_corner().y() && a.max_corner().y() == b.max_corner().y();
}

bool mapComparator(const Lef::via_type::layer_map_type::value_type& a, const Lef::via_type::layer_map_type::value_type& b)
{
    return a.first == b.first && std::is_permutation(a.second.begin(), a.second.end(), b.second.begin(), boxComparator);
}

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
        CHECK(first_layer.direction() == Lef::layer_type::direction_type::HORIZONTAL);
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

TEST_CASE("lef: ispd18_sample.input.lef parsing", "[parser][lef][sample][ispd18]")
{
    auto sample = Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};

    SECTION("Database units are correct", "[parser][lef][sample][ratio]")
    {
        CHECK(sample.micrometer_to_dbu_ratio() == 2000.0);
    }

    SECTION("Sites are parsed correctly", "[parser][lef][sample][ispd18][sites]")
    {
        CHECK( sample.sites().size() == 1 );

        auto core = sample.sites().front();
        CHECK(core.name() == "CoreSite");
        CHECK(core.class_name() == "CORE");
        CHECK(core.width() == micron_t{0.2});
        CHECK(core.height() == micron_t{1.71});
    }

    SECTION("Layers are parsed correctly", "[parser][lef][sample][ispd18][layers]")
    {
        CHECK(sample.layers().size() == 18);

        auto& first_layer = sample.layers().front();
        CHECK(first_layer.name() == "Metal1");
        CHECK(first_layer.type() == Lef::layer_type::type_type::ROUTING);
        CHECK(first_layer.direction() == Lef::layer_type::direction_type::HORIZONTAL);
        CHECK(first_layer.pitch() == micron_t{0.19});
        CHECK(first_layer.offset() == micron_t{0.0});
        CHECK(first_layer.width() == micron_t{0.06});

        CHECK(first_layer.min_width() == micron_t{0.06});
        CHECK(first_layer.area() == micron_t{0.02});
        CHECK(first_layer.spacing() == micron_t{0.06});

        SECTION("EOL Layers are parsed correctly", "[parser][lef][sample][ispd18][layers][EOL]")
        {
            auto& eol = first_layer.EOL();
            CHECK(eol.space() == micron_t{0.09});
            CHECK(eol.width() == micron_t{0.09});
            CHECK(eol.within() == micron_t{0.025});
        }

        SECTION("ParallelRunLength Layers are parsed correctly", "[parser][lef][sample][ispd18][layers][ParallelRunLength]")
        {
            auto& parallelRunLength = first_layer.parallelRunLength();
            CHECK(parallelRunLength.numLength() == 1);
            CHECK(parallelRunLength.numWidth() == 4);
            auto widths = Lef::layer_type::parallelRunLength_type::micrometer_container_type{
                    micron_t{0.0},
                    micron_t{0.1},
                    micron_t{0.75},
                    micron_t{1.5},
            };
            CHECK(std::is_permutation(widths.begin(), widths.end(), parallelRunLength.widths().begin()));
            auto lengths = Lef::layer_type::parallelRunLength_type::micrometer_container_type{
                    micron_t{0.0},
            };
            CHECK(std::is_permutation(lengths.begin(), lengths.end(), parallelRunLength.lengths().begin()));
            auto values = Lef::layer_type::parallelRunLength_type::parallelRunLength_container_type{
                    {{micron_t{0.0},  micron_t{0.0}}, micron_t{0.06}},
                    {{micron_t{0.1},  micron_t{0.0}}, micron_t{0.1}},
                    {{micron_t{0.75}, micron_t{0.0}}, micron_t{0.25}},
                    {{micron_t{1.5},  micron_t{0.0}}, micron_t{0.45}},
            };
            CHECK(std::is_permutation(values.begin(), values.end(), parallelRunLength.values().begin()));
        }
    }

    SECTION("Vias are parsed correctly", "[parser][lef][sample][ispd18][via]")
    {
        CHECK(sample.vias().size() == 22);
        auto& via = sample.vias().front();

        CHECK(via.name() == "VIA12_1C");
        using box = Lef::via_type::micrometer_box_type;
        using point = Lef::via_type::micrometer_point_type;
        using layer = Lef::via_type::layer_container_type;
        auto layers = Lef::via_type::layer_map_type{
            {"Metal1", layer{box{{micron_t{-0.065}, micron_t{-0.035}}, {micron_t{0.065}, micron_t{0.035}}}}},
            {"Via1",   layer{box{{micron_t{-0.035}, micron_t{-0.035}}, {micron_t{0.035}, micron_t{0.035}}}}},
            {"Metal2", layer{box{{micron_t{-0.035}, micron_t{-0.065}}, {micron_t{0.035}, micron_t{0.065}}}}}
        };

        CHECK(std::is_permutation(layers.begin(), layers.end(), via.layers().begin(), mapComparator));
    }

    SECTION("Macros are parsed correctly", "[parser][lef][sample][ispd18][macros]")
    {
        CHECK( sample.macros().size() == 16 );

        auto& first_macro = sample.macros().front();

        CHECK(first_macro.name() == "AOI221X2");
        CHECK(first_macro.class_name() == "CORE");

        CHECK(first_macro.size().x() == micron_t{2.6});
        CHECK(first_macro.size().y() == micron_t{1.71});

        CHECK(first_macro.foreign().name == "AOI221X2");
        CHECK(first_macro.foreign().offset.x() == micron_t{0.0});
        CHECK(first_macro.foreign().offset.y() == micron_t{0.0});

        CHECK(first_macro.origin().x() == micron_t{0.0});
        CHECK(first_macro.origin().y() == micron_t{0.0});

        CHECK(first_macro.site() == "CoreSite");

        CHECK(first_macro.pins().size() == 8);

        auto& first_pin_fm = first_macro.pins().front();

        CHECK(first_pin_fm.name() == "A0");
        CHECK(first_pin_fm.direction() == Lef::macro_type::pin_type::direction_type::INPUT);

        auto rects = first_pin_fm.ports().at(sample.layers().front().name());

        CHECK(rects.front().min_corner().x() == micron_t{0.235});
        CHECK(rects.front().min_corner().y() == micron_t{0.625});
        CHECK(rects.front().max_corner().x() == micron_t{0.365});
        CHECK(rects.front().max_corner().y() == micron_t{0.715});

        CHECK(rects.back().min_corner().x() == micron_t{0.235});
        CHECK(rects.back().min_corner().y() == micron_t{0.625});
        CHECK(rects.back().max_corner().x() == micron_t{0.96});
        CHECK(rects.back().max_corner().y() == micron_t{0.69});

        CHECK(first_macro.obstructions().empty());
    }
}

