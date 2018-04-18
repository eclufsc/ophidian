#include <catch.hpp>
#include <ophidian/floorplan/LefDef2Floorplan.h>

using namespace ophidian::floorplan;
using dbu_t = ophidian::util::database_unit_t;

namespace {
bool operator==(const ophidian::util::LocationDbu& a, const ophidian::util::LocationDbu& b) noexcept
{
    return a.x() == b.x() && a.y() == b.y();
}

bool operator!=(const ophidian::util::LocationDbu& a, const ophidian::util::LocationDbu& b) noexcept
{
    return !(a == b);
}
}

// namespace
// {
// class LefDef2FloorplanFixture
// {
// public:
//     LefDef2FloorplanFixture() {
//         def = std::make_unique<ophidian::parser::Def>("./input_files/simple/simple.def");
//         lef = std::make_unique<ophidian::parser::Lef>("./input_files/simple/simple.lef");
//     }
//
//     std::unique_ptr<ophidian::parser::Def> def;
//     std::unique_ptr<ophidian::parser::Lef> lef;
//     Floorplan floorplan;
// };
// } // namespace
//
// TEST_CASE_METHOD(LefDef2FloorplanFixture,"LefDef2Floorplan: Test for some simple features.", "[floorplan][lef][def][floorplan]")
// {
//     lefDef2Floorplan(*lef, *def, floorplan);
//     REQUIRE(floorplan.chipOrigin() == ophidian::util::LocationDbu{dbu_t{0}, dbu_t{0}});
//     REQUIRE(floorplan.chipUpperRightCorner() == ophidian::util::LocationDbu{dbu_t{27360}, dbu_t{13680}});
//     REQUIRE(floorplan.siteUpperRightCorner(floorplan.find("core")) == ophidian::util::LocationDbu{dbu_t{0.19}*lef->micrometer_to_dbu_ratio(), dbu_t{1.71}*lef->micrometer_to_dbu_ratio()});
//     REQUIRE(floorplan.sitesRange().size() == 1);
//     REQUIRE(floorplan.rowsRange().size() == 4);
// }
