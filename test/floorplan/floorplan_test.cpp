#include <catch.hpp>
#include <ophidian/floorplan/Floorplan.h>

using namespace ophidian::floorplan;

TEST_CASE("Floorplan: Brand-new floorplan must be empty.", "[floorplan][Floorplan]")
{
    ophidian::util::Location origin(0.0, 0.0);
    ophidian::util::Location boundaries(0.0, 0.0);

    Floorplan floorplan;
    REQUIRE(floorplan.chipOrigin() == origin);
    REQUIRE(floorplan.chipUpperRightCorner() == boundaries);
    REQUIRE(floorplan.sites_range().empty());
    REQUIRE(floorplan.rows_range().empty());
}

TEST_CASE("Floorplan: Set Origing and boundaries.", "[floorplan][Floorplan]")
{
    ophidian::util::Location origin(2.0, 3.0);
    ophidian::util::Location boundaries(4000.0, 3000.0);

    Floorplan floorplan;
    floorplan.chipOrigin(origin);
    floorplan.chipUpperRightCorner(boundaries);
    REQUIRE(floorplan.chipOrigin() == origin);
    REQUIRE(floorplan.chipUpperRightCorner() == boundaries);
}

namespace {
class SitesWithPropertiesFixture {
public:
    std::string name1, name2, name3;
    ophidian::util::Location loc1, loc2, loc3;

    SitesWithPropertiesFixture() {
        name1 = "core1";
        name2 = "core2";
        name3 = "core3";

        loc1 = ophidian::util::Location(0.19, 1.71);
        loc2 = ophidian::util::Location(0.20, 1.72);
        loc3 = ophidian::util::Location(0.21, 1.73);
    }
};
}

TEST_CASE_METHOD(SitesWithPropertiesFixture,"Floorplan: Add/Erase Sites.", "[floorplan][Floorplan]")
{
    Floorplan floorplan;
    auto siteRet1 = floorplan.add(Site(), name1, loc1);
    auto siteRet2 = floorplan.add(Site(), name2, loc2);
    auto siteRet3 = floorplan.add(Site(), name3, loc3);

    REQUIRE(floorplan.sites_range().size() == 3);
    REQUIRE(floorplan.name(siteRet1) == name1);
    REQUIRE(floorplan.name(siteRet2) == name2);
    REQUIRE(floorplan.name(siteRet3) == name3);

    REQUIRE(floorplan.siteUpperRightCorner(siteRet1) == loc1);
    REQUIRE(floorplan.siteUpperRightCorner(siteRet2) == loc2);
    REQUIRE(floorplan.siteUpperRightCorner(siteRet3) == loc3);

    floorplan.erase(siteRet1);
    floorplan.erase(siteRet2);
    floorplan.erase(siteRet3);
    REQUIRE(floorplan.sites_range().empty());
}

namespace {
class RowWithPropertiesFixture {
public:
    ophidian::util::Location origin1, origin2, origin3;
    size_t numSites1, numSites2, numSites3;

    SitesWithPropertiesFixture sitesSystem;

    RowWithPropertiesFixture() {
        origin1 = ophidian::util::Location(10.0, 10.0);
        origin2 = ophidian::util::Location(200.0, 200.0);
        origin3 = ophidian::util::Location(400.0, 400.0);

        numSites1 = 100;
        numSites2 = 150;
        numSites3 = 200;
    }
};
}

TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Add/Erase Rows.", "[floorplan][Floorplan]")
{
    Floorplan floorplan;
    auto site1 = floorplan.add(Site(), sitesSystem.name1, sitesSystem.loc1);
    auto site2 = floorplan.add(Site(), sitesSystem.name2, sitesSystem.loc2);
    auto site3 = floorplan.add(Site(), sitesSystem.name3, sitesSystem.loc3);

    auto rowRet1 = floorplan.add(Row(), origin1, numSites1, site1);
    auto rowRet2 = floorplan.add(Row(), origin2, numSites2, site2);
    auto rowRet3 = floorplan.add(Row(), origin3, numSites3, site3);

    REQUIRE(floorplan.rows_range().size() == 3);
    REQUIRE(floorplan.origin(rowRet1) == origin1);
    REQUIRE(floorplan.origin(rowRet2) == origin2);
    REQUIRE(floorplan.origin(rowRet3) == origin3);

    REQUIRE(floorplan.numberOfSites(rowRet1) == numSites1);
    REQUIRE(floorplan.numberOfSites(rowRet2) == numSites2);
    REQUIRE(floorplan.numberOfSites(rowRet3) == numSites3);

    REQUIRE(floorplan.site(rowRet1) == site1);
    REQUIRE(floorplan.site(rowRet2) == site2);
    REQUIRE(floorplan.site(rowRet3) == site3);

    floorplan.erase(rowRet1);
    floorplan.erase(rowRet2);
    floorplan.erase(rowRet3);
    REQUIRE(floorplan.rows_range().empty());
}

TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Row Upper Right Corner.", "[floorplan][Floorplan]")
{
    Floorplan floorplan;
    auto siteRet1 = floorplan.add(Site(), sitesSystem.name1, sitesSystem.loc1);
    auto siteRet2 = floorplan.add(Site(), sitesSystem.name2, sitesSystem.loc2);
    auto rowRet1 = floorplan.add(Row(), origin1, numSites1, siteRet1);
    auto rowRet2 = floorplan.add(Row(), origin2, numSites2, siteRet2);

    ophidian::util::Location urCorner1(0.19*100, 1.71);
    REQUIRE(floorplan.rowUpperRightCorner(rowRet1) == urCorner1);

    ophidian::util::Location urCorner2(0.20*150, 1.72);
    REQUIRE(floorplan.rowUpperRightCorner(rowRet2) == urCorner2);
}
