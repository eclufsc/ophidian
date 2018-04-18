#include <catch.hpp>
#include <ophidian/floorplan/Floorplan.h>
#include <ophidian/util/Units.h>

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

TEST_CASE("Floorplan: Brand-new floorplan must be empty.", "[floorplan]")
{
	ophidian::util::LocationDbu origin{dbu_t{0.0}, dbu_t{0.0}};
	ophidian::util::LocationDbu boundaries{dbu_t{0.0}, dbu_t{0.0}};

	Floorplan floorplan;
	REQUIRE(floorplan.chipOrigin() == origin);
	REQUIRE(floorplan.chipUpperRightCorner() == boundaries);
	REQUIRE(floorplan.sitesRange().empty());
	REQUIRE(floorplan.rowsRange().empty());
}

TEST_CASE("Floorplan: Set Origing and boundaries.", "[floorplan]")
{
	ophidian::util::LocationDbu origin{dbu_t{2.0}, dbu_t{3.0}};
	ophidian::util::LocationDbu boundaries{dbu_t{4000.0}, dbu_t{3000.0}};

	Floorplan floorplan;
	floorplan.chipOrigin(origin);
	floorplan.chipUpperRightCorner(boundaries);
	REQUIRE(floorplan.chipOrigin() == origin);
	REQUIRE(floorplan.chipUpperRightCorner() == boundaries);
}

namespace
{
class SitesWithPropertiesFixture
{
public:
	std::string name1, name2, name3;
	ophidian::util::LocationDbu loc1, loc2, loc3;

	SitesWithPropertiesFixture() {
		name1 = "core1";
		name2 = "core2";
		name3 = "core3";

		loc1 = ophidian::util::LocationDbu{dbu_t{0.19}, dbu_t{1.71}};
		loc2 = ophidian::util::LocationDbu{dbu_t{0.20}, dbu_t{1.72}};
		loc3 = ophidian::util::LocationDbu{dbu_t{0.21}, dbu_t{1.73}};
	}
};
} // namespace
  //

// TEST_CASE_METHOD(SitesWithPropertiesFixture,"Floorplan: Add/Erase Sites.", "[floorplan]")
// {
//     Floorplan floorplan;
//     auto siteRet1 = floorplan.add(Site(), name1, loc1);
//     auto siteRet2 = floorplan.add(Site(), name2, loc2);
//     auto siteRet3 = floorplan.add(Site(), name3, loc3);
//
//     REQUIRE(floorplan.sitesRange().size() == 3);
//     REQUIRE(floorplan.name(siteRet1) == name1);
//     REQUIRE(floorplan.name(siteRet2) == name2);
//     REQUIRE(floorplan.name(siteRet3) == name3);
//
//     REQUIRE(floorplan.siteUpperRightCorner(siteRet1) == loc1);
//     REQUIRE(floorplan.siteUpperRightCorner(siteRet2) == loc2);
//     REQUIRE(floorplan.siteUpperRightCorner(siteRet3) == loc3);
//
//     floorplan.erase(siteRet1);
//     floorplan.erase(siteRet2);
//     floorplan.erase(siteRet3);
//     REQUIRE(floorplan.sitesRange().empty());
// }

// TEST_CASE_METHOD(SitesWithPropertiesFixture,"Floorplan: Test site mapping.", "[floorplan]")
// {
//     Floorplan floorplan;
//     floorplan.add(Site(), name1, loc1);
//     auto site1 = floorplan.find(name1);
//     REQUIRE(floorplan.sitesRange().size() == 1);
//     floorplan.erase(site1);
//     REQUIRE(floorplan.sitesRange().size() == 0);
// }
//
// namespace
// {
// class RowWithPropertiesFixture
// {
// public:
//     ophidian::util::LocationDbu origin1, origin2, origin3;
//     size_t numSites1, numSites2, numSites3;
//
//     SitesWithPropertiesFixture sitesSystem;
//
//     RowWithPropertiesFixture() {
//         origin1 = ophidian::util::LocationDbu{dbu_t{10.0}, dbu_t{10.0}};
//         origin2 = ophidian::util::LocationDbu{dbu_t{200.0}, dbu_t{200.0}};
//         origin3 = ophidian::util::LocationDbu{dbu_t{400.0}, dbu_t{400.0}};
//
//         numSites1 = 100;
//         numSites2 = 150;
//         numSites3 = 200;
//     }
// };
// } // namespace
//   //
//
// TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Add/Erase Rows.", "[floorplan]")
// {
//     Floorplan floorplan;
//     auto site1 = floorplan.add(Site(), sitesSystem.name1, sitesSystem.loc1);
//     auto site2 = floorplan.add(Site(), sitesSystem.name2, sitesSystem.loc2);
//     auto site3 = floorplan.add(Site(), sitesSystem.name3, sitesSystem.loc3);
//
//     auto rowRet1 = floorplan.add(Row(), origin1, numSites1, site1);
//     auto rowRet2 = floorplan.add(Row(), origin2, numSites2, site2);
//     auto rowRet3 = floorplan.add(Row(), origin3, numSites3, site3);
//
//     REQUIRE(floorplan.rowsRange().size() == 3);
//     REQUIRE(floorplan.origin(rowRet1) == origin1);
//     REQUIRE(floorplan.origin(rowRet2) == origin2);
//     REQUIRE(floorplan.origin(rowRet3) == origin3);
//
//     REQUIRE(floorplan.numberOfSites(rowRet1) == numSites1);
//     REQUIRE(floorplan.numberOfSites(rowRet2) == numSites2);
//     REQUIRE(floorplan.numberOfSites(rowRet3) == numSites3);
//
//     REQUIRE(floorplan.site(rowRet1) == site1);
//     REQUIRE(floorplan.site(rowRet2) == site2);
//     REQUIRE(floorplan.site(rowRet3) == site3);
//
//     floorplan.erase(rowRet1);
//     floorplan.erase(rowRet2);
//     floorplan.erase(rowRet3);
//     REQUIRE(floorplan.rowsRange().empty());
// }
//
// TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Row Upper Right Corner.", "[floorplan]")
// {
//     Floorplan floorplan;
//     auto siteRet1 = floorplan.add(Site(), sitesSystem.name1, sitesSystem.loc1);
//     auto siteRet2 = floorplan.add(Site(), sitesSystem.name2, sitesSystem.loc2);
//     auto rowRet1 = floorplan.add(Row(), origin1, numSites1, siteRet1);
//     auto rowRet2 = floorplan.add(Row(), origin2, numSites2, siteRet2);
//
//     ophidian::util::LocationDbu urCorner1{dbu_t{0.19*100}, dbu_t{1.71}};
//     REQUIRE(floorplan.rowUpperRightCorner(rowRet1) == urCorner1);
//
//     ophidian::util::LocationDbu urCorner2{dbu_t{0.20*150}, dbu_t{1.72}};
//     REQUIRE(floorplan.rowUpperRightCorner(rowRet2) == urCorner2);
// }
