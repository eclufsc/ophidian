#include <catch.hpp>
#include <vector>

#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/LibraryFactory.h>

using namespace ophidian::parser;

TEST_CASE("Library Factory Test", "[routing][library][factory]")
{
    using namespace ophidian::util;
    using unit_type = double;
    using strategy = floor_strategy<unit_type, unit_type, unit_type>;
    using table = LookupTable<unit_type, unit_type, unit_type, strategy>;
    using contents = tableContents<unit_type, unit_type, unit_type>;

    Def sample_def = ophidian::parser::Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};
    Lef sample_lef = ophidian::parser::Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};

    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_ispd2018(design, sample_def, sample_lef);

    auto& library = design.routing_library();

    SECTION("Layers are created correctly", "[routing][library][factory]")
    {
        CHECK(library.size_layer() == 18);
        auto layer = library.find_layer_instance("Metal1");

        CHECK(library.name(layer) == "Metal1");
        CHECK(library.type(layer) == ophidian::routing::LayerType::ROUTING);
        CHECK(library.direction(layer) == ophidian::routing::LayerDirection::HORIZONTAL);
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

//    SECTION("Via are created correctly", "[routing][library][factory]")
//    {

//    }

//    SECTION("Tracks are created correctly", "[routing][library][factory]")
//    {

//    }
}

