#include <catch.hpp>
#include <ophidian/util/DefWriter.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/LibraryFactory.h>

using dbu_t = ophidian::util::database_unit_t;


using namespace ophidian::design;


TEST_CASE("DefWriter test", "[util]") {
    ophidian::parser::Def sample_def = ophidian::parser::Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};
    ophidian::parser::Lef sample_lef = ophidian::parser::Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};
    ophidian::parser::Guide sample_guide = ophidian::parser::Guide{"input_files/ispd18/ispd18_sample/ispd18_sample.input.guide"};
    Design design;
    ophidian::design::factory::make_design_ispd2018(design, sample_def, sample_lef, sample_guide);
    ophidian::util::write_def(design, "Def_test.def");
}
