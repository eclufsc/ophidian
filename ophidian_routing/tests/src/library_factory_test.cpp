#include <catch2/catch.hpp>
#include <vector>

#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/LibraryFactory.h>

using namespace ophidian::parser;

TEST_CASE("Routing Library Factory Test ispd2018", "[routing][library][factory][ispd18]")
{
    using namespace ophidian::util;
    using unit_type = double;
    using strategy = FloorStrategy<unit_type, unit_type, unit_type>;
    using table = LookupTable<unit_type, unit_type, unit_type, strategy>;
    using contents = TableContents<unit_type, unit_type, unit_type>;

    Def sample_def = ophidian::parser::Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};
    Lef sample_lef = ophidian::parser::Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};
    Guide sample_guide = ophidian::parser::Guide{"input_files/ispd18/ispd18_sample/ispd18_sample.input.guide"};

    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_ispd2018(design, sample_def, sample_lef, sample_guide);

    auto& library = design.routing_library();

    SECTION("Layers are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_layer() == 18);
        auto layer = library.find_layer_instance("Metal1");

        CHECK(library.name(layer) == "Metal1");
        CHECK(library.type(layer) == ophidian::routing::LayerType::ROUTING);
        CHECK(library.direction(layer) == ophidian::routing::Direction::HORIZONTAL);
        CHECK(library.pitch(layer) == database_unit_t{380});
        CHECK(library.offset(layer) == database_unit_t{0.0});
        CHECK(library.width(layer) == database_unit_t{120});
        CHECK(library.minWidth(layer) == database_unit_t{120});
        CHECK(library.area(layer) == database_unit_t{40});
        CHECK(library.spacing(layer) == database_unit_t{120});
        CHECK(library.EOLspace(layer) == database_unit_t{180});
        CHECK(library.EOLwidth(layer) == database_unit_t{180});
        CHECK(library.EOLwithin(layer) == database_unit_t{50});

        auto& spacing_table = library.spacing_table(layer);
        CHECK(spacing_table.column_values().size() == 1);
        CHECK(spacing_table.row_values().size() == 4);
        CHECK(spacing_table.compute(database_unit_t{1500}, database_unit_t{0}) == database_unit_t{200});
    }

    SECTION("Via are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_via() == 22);
        auto via = library.find_via_instance("VIA12_1C_H");
        auto box = *library.geometry(via, "Metal2").begin();
        CHECK(box.min_corner().x() == database_unit_t{-130});
        CHECK(box.min_corner().y() == database_unit_t{-70});
        CHECK(box.max_corner().x() == database_unit_t{130});
        CHECK(box.max_corner().y() == database_unit_t{70});
    }

    SECTION("Tracks are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_track() == 18);
        auto track = *library.begin_track();
        CHECK(library.direction(track) == ophidian::routing::Direction::VERTICAL);
        CHECK(library.start(track) == database_unit_t{83800});
        CHECK(library.numTracs(track) == 52);
        CHECK(library.space(track) == database_unit_t{400});
        CHECK(library.name(library.layer(track)) == "Metal9");
    }
}
TEST_CASE("Routing Library Factory Test ispd2019", "[routing][library][factory][ispd19]")
{
    using namespace ophidian::util;
    using unit_type = double;
    using strategy = FloorStrategy<unit_type, unit_type, unit_type>;
    using table = LookupTable<unit_type, unit_type, unit_type, strategy>;
    using contents = TableContents<unit_type, unit_type, unit_type>;

    Def sample_def = ophidian::parser::Def{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.def"};
    Lef sample_lef = ophidian::parser::Lef{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.lef"};
    Guide sample_guide = ophidian::parser::Guide{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.guide"};

    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_ispd2019(design, sample_def, sample_lef, sample_guide);

    auto& library = design.routing_library();

    SECTION("Layers are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_layer() == 18);
        auto layer = library.find_layer_instance("Metal2");

        CHECK(library.name(layer) == "Metal2");
        CHECK(library.type(layer) == ophidian::routing::LayerType::ROUTING);
        CHECK(library.direction(layer) == ophidian::routing::Direction::HORIZONTAL);
        CHECK(library.pitch(layer) == database_unit_t{200});
        CHECK(library.offset(layer) == database_unit_t{0.0});
        CHECK(library.width(layer) == database_unit_t{100});
        CHECK(library.minWidth(layer) == database_unit_t{100});
        CHECK(library.area(layer) == database_unit_t{28});
        CHECK(library.spacing(layer) == database_unit_t{0});
        CHECK(library.EOLspace(layer) == database_unit_t{160});
        CHECK(library.EOLwidth(layer) == database_unit_t{160});
        CHECK(library.EOLwithin(layer) == database_unit_t{50});
        CHECK(library.EOLPspace(layer) == database_unit_t{200});
        CHECK(library.EOLPwidth(layer) == database_unit_t{160});
        CHECK(library.EOLPwithin(layer) == database_unit_t{50});
        CHECK(library.EOLpar_space(layer) == database_unit_t{200});
        CHECK(library.EOLpar_within(layer) == database_unit_t{50});

        CHECK(library.direction(library.prefTrack(layer)) == ophidian::routing::Direction::HORIZONTAL);
        CHECK(library.direction(library.nonPrefTrack(layer)) == ophidian::routing::Direction::VERTICAL);
        CHECK(library.name((library.upperLayer(layer))) == "Metal3");
//        CHECK(library.layerIndex(layer) == 2);// See Renan's comment in Library.cpp
        CHECK(library.isFirstRightBelowSecond(library.find_layer_instance("Metal1"), library.find_layer_instance("Metal2")));

        auto& spacing_table = library.spacing_table(layer);
        CHECK(spacing_table.column_values().size() == 5);
        CHECK(spacing_table.row_values().size() == 30);
        CHECK(spacing_table.compute(database_unit_t{0}, database_unit_t{0}) == database_unit_t{100});

        layer = library.find_layer_instance("Via2");
        CHECK(library.name(layer) == "Via2");
        CHECK(library.type(layer) == ophidian::routing::LayerType::CUT);
        CHECK(library.spacing(layer) == database_unit_t{150});
        CHECK(library.width(layer) == database_unit_t{100});
        CHECK(library.adjacentSpacing(layer) == database_unit_t{310});
        CHECK(library.adjacentCuts(layer) == 3);
        CHECK(library.cutWithinLength(layer) == database_unit_t{400});
    }

    SECTION("Via are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_via() == 67);
        auto via = library.find_via_instance("VIA12_1C_H");
        auto box = *library.geometry(via, "Metal2").begin();
        CHECK(box.min_corner().x() == database_unit_t{-110});
        CHECK(box.min_corner().y() == database_unit_t{-50});
        CHECK(box.max_corner().x() == database_unit_t{110});
        CHECK(box.max_corner().y() == database_unit_t{50});
    }

    SECTION("Tracks are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_track() == 18);
        auto track = *library.begin_track();
        CHECK(library.direction(track) == ophidian::routing::Direction::HORIZONTAL);
        CHECK(library.start(track) == database_unit_t{200});
        CHECK(library.numTracs(track) == 975);
        CHECK(library.space(track) == database_unit_t{400});
        CHECK(library.name(library.layer(track)) == "Metal9");
    }
}

TEST_CASE("Make a routing library from an iccad2020 file.", "[routing][library][factory][iccad2020]")
{
    ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_iccad2020(design, sample);

    auto & library = design.routing_library();
    auto & std_cells = design.standard_cells();

    SECTION("Test for highest layer.")
    {
        REQUIRE(library.size_layer() == 3);
        auto highest_layer = library.highest_layer();
        REQUIRE(library.name(highest_layer) == "M3");
    }

    SECTION("Test for lower layer.")
    {
        // Not working!
        // auto layer_2 = library.lowerLayer(highest_layer);
        // REQUIRE(library.name(layer_2) == "M2");
    }


    SECTION("Test for upper layer.")
    {
        // Not working!
        // auto layer_1 = library.find_layer_instance("M1");
        // REQUIRE(library.type(layer_1) == ophidian::routing::LayerType::ROUTING);
        // auto upperLayer = library.upperLayer(layer_1);
        // REQUIRE(library.name(upperLayer) == "M2");
    }

    SECTION("Test for layer directions.")
    {
        auto layer_1 = library.find_layer_instance("M1");
        REQUIRE(library.type(layer_1) == ophidian::routing::LayerType::ROUTING);
        REQUIRE(library.layerIndex(layer_1) == 1);

        //metal 1 is always horizontal
        REQUIRE(library.direction(layer_1) == ophidian::routing::Direction::HORIZONTAL);
        auto layer_2 = library.find_layer_instance("M2");
        REQUIRE(library.direction(layer_2) == ophidian::routing::Direction::VERTICAL);
        auto highest_layer = library.highest_layer();
        REQUIRE(library.direction(highest_layer) == ophidian::routing::Direction::HORIZONTAL);
    }

    SECTION("Blockages size.")
    {
        REQUIRE(library.size_blockage() == 2);
        unsigned int i = 0;
        for(auto blk_it = library.begin_blockages(); blk_it != library.end_blockages(); blk_it++)
            i++;
        REQUIRE(i == 2);
    }

    SECTION("Blockage getters.")
    {
        auto blk_B1 = library.find_blockage("B1");
        REQUIRE(library.name(blk_B1) == "B1");
    }

    SECTION("Blockage properties.")
    {
        auto blk_B1 = library.find_blockage("B1");
        REQUIRE(library.demand(blk_B1) == 2);
        auto layer_B1 = library.layer(blk_B1);
        REQUIRE(library.name(layer_B1) == "M1");
    }

    SECTION("Blockage composition.")
    {
        auto blk_B1 = library.find_blockage("B1");
        auto std_cell_MC1 = library.std_cell(blk_B1);
        REQUIRE(std_cells.name(std_cell_MC1) == "MC1");
        auto blockages = library.blockages(std_cell_MC1);
        REQUIRE(blockages.size() == 2);
    }
}
