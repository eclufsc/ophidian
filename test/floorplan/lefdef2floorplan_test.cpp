#include <catch.hpp>
#include <ophidian/floorplan/LefDef2Floorplan.h>

using namespace ophidian::floorplan;

namespace
{
class LefDef2FloorplanFixture
{
public:
	LefDef2FloorplanFixture() {
		ophidian::parser::DefParser defParser;
		def = std::move(defParser.readFile("./input_files/simple.def"));
		ophidian::parser::LefParser lefParser;
		lef = std::move(lefParser.readFile("./input_files/simple.lef"));
	}

	std::unique_ptr<ophidian::parser::Def> def;
	std::unique_ptr<ophidian::parser::Lef> lef;
	Floorplan floorplan;
};
} // namespace
  //

TEST_CASE_METHOD(LefDef2FloorplanFixture,"LefDef2Floorplan: Test for some simple features.", "[floorplan][lef][def][floorplan]")
{
	lefDef2Floorplan(*lef, *def, floorplan);
	REQUIRE(floorplan.chipOrigin() == ophidian::util::LocationDbu(0, 0));
	REQUIRE(floorplan.chipUpperRightCorner() == ophidian::util::LocationDbu(27360, 13680));
	REQUIRE(floorplan.siteUpperRightCorner(floorplan.find("core")) == ophidian::util::LocationDbu(0.19*lef->databaseUnits(), 1.71*lef->databaseUnits()));
	REQUIRE(floorplan.sitesRange().size() == 1);
	REQUIRE(floorplan.rowsRange().size() == 4);
}
