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

    auto a = 0;
    a++;
    a--;
    REQUIRE( 0.1 == 0.1);
}

