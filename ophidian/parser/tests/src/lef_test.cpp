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

        SECTION("end_of_line Layers are parsed correctly", "[parser][lef][sample][ispd18][layers][end_of_line]")
        {
            auto& eol = first_layer.end_of_line();
            CHECK(eol.space() == micron_t{0.09});
            CHECK(eol.width() == micron_t{0.09});
            CHECK(eol.within() == micron_t{0.025});
        }

        SECTION("ParallelRunLength Layers are parsed correctly", "[parser][lef][sample][ispd18][layers][ParallelRunLength]")
        {
            auto& parallel_run_length = first_layer.parallel_run_length();
            CHECK(parallel_run_length.lengths().size() == 1);
            CHECK(parallel_run_length.widths().size() == 4);
            auto widths = Lef::layer_type::parallel_run_length_type::width_container_type{
                    micron_t{0.0},
                    micron_t{0.1},
                    micron_t{0.75},
                    micron_t{1.5},
            };
            CHECK(std::is_permutation(widths.begin(), widths.end(), parallel_run_length.widths().begin()));
            auto lengths = Lef::layer_type::parallel_run_length_type::length_container_type{
                    micron_t{0.0},
            };
            CHECK(std::is_permutation(lengths.begin(), lengths.end(), parallel_run_length.lengths().begin()));
            auto values = Lef::layer_type::parallel_run_length_type::spacing_container_type{
                    {{micron_t{0.0},  micron_t{0.0}}, micron_t{0.06}},
                    {{micron_t{0.1},  micron_t{0.0}}, micron_t{0.1}},
                    {{micron_t{0.75}, micron_t{0.0}}, micron_t{0.25}},
                    {{micron_t{1.5},  micron_t{0.0}}, micron_t{0.45}},
            };
            CHECK(std::is_permutation(values.begin(), values.end(), parallel_run_length.width_length_to_spacing().begin()));
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

TEST_CASE("lef: ispd19_sample4.input.lef parsing", "[parser][lef][sample][ispd19]"){
    
    auto sample = Lef{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.lef"};

    SECTION("Database units are correct", "[parser][lef][sample][ratio]")
    {
        CHECK(sample.micrometer_to_dbu_ratio() == 2000.0);
    }

    SECTION("Sites are parsed correctly", "[parser][lef][sample][ispd19][sites]")
    {
        CHECK( sample.sites().size() == 3 );

        auto core = sample.sites().front();
        CHECK(core.name() == "CoreSite");
        CHECK(core.class_name() == "CORE");
        CHECK(core.width() == micron_t{0.1});
        CHECK(core.height() == micron_t{1.2});
    }

    SECTION("Layers are parsed correctly", "[parser][lef][sample][ispd19][layers]")
    {
        CHECK(sample.layers().size() == 18);

        auto& first_layer = sample.layers().front();
        CHECK(first_layer.name() == "Metal1");
        CHECK(first_layer.type() == Lef::layer_type::type_type::ROUTING);
        CHECK(first_layer.direction() == Lef::layer_type::direction_type::VERTICAL);
        CHECK(first_layer.pitch() == micron_t{0.1});
        CHECK(first_layer.offset() == micron_t{0});
        CHECK(first_layer.width() == micron_t{0.05});

        CHECK(first_layer.min_width() == micron_t{0.05});
        CHECK(first_layer.area() == micron_t{0.0115});
        CHECK(first_layer.spacing() == micron_t{0});

        SECTION("end_of_line Layers are parsed correctly", "[parser][lef][sample][ispd19][layers][end_of_line]")
        {
            auto& eol = first_layer.end_of_line();
            CHECK(eol.space() == micron_t{0.06});
            CHECK(eol.width() == micron_t{0.06});
            CHECK(eol.within() == micron_t{0.025});
            
                SECTION("parallel edge rules are parsed correctly",
                "[parser][lef][sample][ispd19][layers][end_of_line][parallelEdge]")
                {
                    // first layer
                    auto& eol_par_0 = first_layer.parallel_edge();
                    CHECK(eol_par_0.space() == micron_t{0});
                    CHECK(eol_par_0.width() == micron_t{0});
                    CHECK(eol_par_0.within() == micron_t{0});
                    CHECK(eol_par_0.par_space() == micron_t{0});
                    CHECK(eol_par_0.par_within() == micron_t{0});
                    // third layer
                    auto& third_layer = sample.layers().at(2);
                    auto& eol_par_2 = third_layer.parallel_edge();
                    CHECK(eol_par_2.space() == micron_t{0.10});
                    CHECK(eol_par_2.width() == micron_t{0.08});
                    CHECK(eol_par_2.within() == micron_t{0.025});
                    CHECK(eol_par_2.par_space() == micron_t{0.10});
                    CHECK(eol_par_2.par_within() == micron_t{0.025});
                    // last layer
                    auto& last_layer = sample.layers().at(12);
                    auto& eol_par_14 = last_layer.parallel_edge();
                    CHECK(eol_par_14.space() == micron_t{0.12});
                    CHECK(eol_par_14.width() == micron_t{0.10});
                    CHECK(eol_par_14.within() == micron_t{0.035});
                    CHECK(eol_par_14.par_space() == micron_t{0.12});
                    CHECK(eol_par_14.par_within() == micron_t{0.035});
                }
        }

        SECTION("ParallelRunLength Layers are parsed correctly", "[parser][lef][sample][ispd19][layers][ParallelRunLength]")
        {
            auto& parallel_run_length = first_layer.parallel_run_length();
            CHECK(parallel_run_length.lengths().size() == 5);
            CHECK(parallel_run_length.widths().size() == 30);
            auto widths = Lef::layer_type::parallel_run_length_type::width_container_type{
                    micron_t{0.0},
                    micron_t{0.10},
                    micron_t{0.28},
                    micron_t{0.47},
                    micron_t{0.63},
                    micron_t{1.50}
            };
            CHECK(std::is_permutation(widths.begin(), widths.end(), parallel_run_length.widths().begin()));
            auto lengths = Lef::layer_type::parallel_run_length_type::length_container_type{
                    micron_t{0.0},
                    micron_t{0.22},
                    micron_t{0.47},
                    micron_t{0.63},
                    micron_t{1.50}
            };
            CHECK(std::is_permutation(lengths.begin(), lengths.end(), parallel_run_length.lengths().begin()));
            auto values = Lef::layer_type::parallel_run_length_type::spacing_container_type{
                {{micron_t{0.00}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{0.00}, micron_t{0.22}}, micron_t{0.05}},  {{micron_t{0.00}, micron_t{0.47}}, micron_t{0.05}},  {{micron_t{0.00}, micron_t{0.63}}, micron_t{0.05}},  {{micron_t{0.00}, micron_t{1.5}}, micron_t{0.05}},
                {{micron_t{0.10}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{0.10}, micron_t{0.22}}, micron_t{0.06}},  {{micron_t{0.10}, micron_t{0.47}}, micron_t{0.06}},  {{micron_t{0.10}, micron_t{0.63}}, micron_t{0.06}},  {{micron_t{0.10}, micron_t{1.5}}, micron_t{0.06}},
                {{micron_t{0.28}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{0.28}, micron_t{0.22}}, micron_t{0.10}},  {{micron_t{0.28}, micron_t{0.47}}, micron_t{0.10}},  {{micron_t{0.28}, micron_t{0.63}}, micron_t{0.10}},  {{micron_t{0.28}, micron_t{1.5}}, micron_t{0.10}},
                {{micron_t{0.47}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{0.47}, micron_t{0.22}}, micron_t{0.10}},  {{micron_t{0.47}, micron_t{0.47}}, micron_t{0.13}},  {{micron_t{0.47}, micron_t{0.63}}, micron_t{0.13}},  {{micron_t{0.47}, micron_t{1.5}}, micron_t{0.13}},
                {{micron_t{0.63}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{0.63}, micron_t{0.22}}, micron_t{0.10}},  {{micron_t{0.63}, micron_t{0.47}}, micron_t{0.13}},  {{micron_t{0.63}, micron_t{0.63}}, micron_t{0.15}},  {{micron_t{0.63}, micron_t{1.5}}, micron_t{0.15}},
                {{micron_t{1.50}, micron_t{0.0}}, micron_t{0.05}},  {{micron_t{1.50}, micron_t{0.22}}, micron_t{0.10}},
            };
           CHECK(std::is_permutation(values.begin(), values.end(), parallel_run_length.width_length_to_spacing().begin()));
        }
   }

    SECTION("Vias are parsed correctly", "[parser][lef][sample][ispd19][via]")
    {
        CHECK(sample.vias().size() == 67);
        auto& via = sample.vias().front();

        CHECK(via.name() == "VIA12_1C");
        using box = Lef::via_type::micrometer_box_type;
        using point = Lef::via_type::micrometer_point_type;
        using layer = Lef::via_type::layer_container_type;
        auto layers = Lef::via_type::layer_map_type{
            {"Metal1", layer{box{{micron_t{-0.025}, micron_t{-0.055}}, {micron_t{0.025}, micron_t{0.055}}}}},
            {"Via1",   layer{box{{micron_t{-0.025}, micron_t{-0.025}}, {micron_t{0.025}, micron_t{0.025}}}}},
            {"Metal2", layer{box{{micron_t{-0.055}, micron_t{-0.025}}, {micron_t{0.055}, micron_t{0.025}}}}}
        };

        CHECK(std::is_permutation(layers.begin(), layers.end(), via.layers().begin(), mapComparator));
    }

    SECTION("Macros are parsed correctly", "[parser][lef][sample][ispd19][macros]")
    {
        CHECK( sample.macros().size() == 27 );

        auto& first_macro = sample.macros().front();

        CHECK(first_macro.name() == "XNOR2X1");
        CHECK(first_macro.class_name() == "CORE");

        CHECK(first_macro.size().x() == micron_t{1.4});
        CHECK(first_macro.size().y() == micron_t{1.2});

        CHECK(first_macro.foreign().name == "XNOR2X1");
        CHECK(first_macro.foreign().offset.x() == micron_t{0.0});
        CHECK(first_macro.foreign().offset.y() == micron_t{0.0});

        CHECK(first_macro.origin().x() == micron_t{0.0});
        CHECK(first_macro.origin().y() == micron_t{0.0});

        CHECK(first_macro.site() == "CoreSite");

        CHECK(first_macro.pins().size() == 5);

        auto& first_pin_fm = first_macro.pins().front();

        CHECK(first_pin_fm.name() == "A");
        CHECK(first_pin_fm.direction() == Lef::macro_type::pin_type::direction_type::INPUT);

        auto rects = first_pin_fm.ports().at(sample.layers().front().name());

        CHECK(rects.front().min_corner().x() == micron_t{0.440});
        CHECK(rects.front().min_corner().y() == micron_t{0.557});
        CHECK(rects.front().max_corner().x() == micron_t{0.542});
        CHECK(rects.front().max_corner().y() == micron_t{0.638});

        CHECK(rects.back().min_corner().x() == micron_t{0.162});
        CHECK(rects.back().min_corner().y() == micron_t{0.442});
        CHECK(rects.back().max_corner().x() == micron_t{0.501});
        CHECK(rects.back().max_corner().y() == micron_t{0.496});

        CHECK(!first_macro.obstructions().empty());
    }
    
    SECTION("Adjacent cut spacing is parsed correctly","[parser][lef][sample][ispd19][adjacent_cut_spacing]")
    {
        auto& layers = sample.layers();
        REQUIRE(layers.size() == 18);
        
        auto first_via_layer = std::next(layers.begin());
        auto& adj_cut_rule = first_via_layer->adjacent_cut_spacing();
        

        REQUIRE(adj_cut_rule.adj_spacing() == micron_t{0});
        REQUIRE(adj_cut_rule.cuts() == 0);
        REQUIRE(adj_cut_rule.cut_within_length() == micron_t{0});
    
        auto second_via_layer = *std::next(std::next(first_via_layer));
        auto& adj_cut_rule2 = second_via_layer.adjacent_cut_spacing();
        
        REQUIRE(adj_cut_rule2.adj_spacing() == micron_t{.155});
        REQUIRE(adj_cut_rule2.cuts() == 3);
        REQUIRE(adj_cut_rule2.cut_within_length() == micron_t{.2});
    }

    SECTION("Corner to corner spacing is parsed correctly", "[parser][lef][sample][ispd19][CornerSpacing]")
    {
        auto& layers = sample.layers();
        
        auto first_corner_spacing_layer = std::next(std::next(layers.begin()));
        auto& begin_corner_spacing = first_corner_spacing_layer->corner_spacing();

        REQUIRE(begin_corner_spacing.eol_width() == micron_t{0.08});

        auto width_to_spacing = begin_corner_spacing.width_to_spacing();
        
        REQUIRE(width_to_spacing.at(0).first == micron_t{0.00});
        REQUIRE(width_to_spacing.at(0).second == micron_t{0.10});
        REQUIRE(width_to_spacing.at(1).first == micron_t{0.20});
        REQUIRE(width_to_spacing.at(1).second == micron_t{0.20});
        REQUIRE(width_to_spacing.at(2).first == micron_t{0.50});
        REQUIRE(width_to_spacing.at(2).second == micron_t{0.30});

        auto last_corner_spacing_layer = layers.at(12);
        auto& end_corner_spacing = last_corner_spacing_layer.corner_spacing();

        REQUIRE(end_corner_spacing.eol_width() == micron_t{0.08});

        width_to_spacing = end_corner_spacing.width_to_spacing();

        REQUIRE(width_to_spacing.at(0).first == micron_t{0.00});
        REQUIRE(width_to_spacing.at(0).second == micron_t{0.10});
        REQUIRE(width_to_spacing.at(1).first == micron_t{0.20});
        REQUIRE(width_to_spacing.at(1).second == micron_t{0.20});
        REQUIRE(width_to_spacing.at(2).first == micron_t{0.50});
        REQUIRE(width_to_spacing.at(2).second == micron_t{0.30});
    }
}

