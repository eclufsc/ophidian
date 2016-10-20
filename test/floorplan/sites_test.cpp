#include <catch.hpp>
#include <ophidian/floorplan/Sites.h>

using namespace ophidian::floorplan;

TEST_CASE("Sites: Brand-new Sites must be empty.", "[floorplan][Sites]")
{
    Sites sites;
    REQUIRE(sites.sites_range().empty());
}

TEST_CASE("Sites: Add Site.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();
    REQUIRE(sites.sites_range().size() == 1);
}

TEST_CASE("Sites: Erase Site.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();
    sites.erase(site);
    REQUIRE(sites.sites_range().size() == 0);
}

TEST_CASE("Sites: Add four Sites.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();
    site = sites.add();
    site = sites.add();
    site = sites.add();
    REQUIRE(sites.sites_range().size() == 4);
}

TEST_CASE("Sites: Add name property.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();

    std::string name = "core";
    sites.name(site, name);
    REQUIRE(sites.name(site) == name);
}

TEST_CASE("Sites: Add dimensions property.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();

    ophidian::geometry::Location loc(ophidian::util::micrometer_t(0.19), ophidian::util::micrometer_t(1.71));
    sites.dimension(site, loc);
    auto loc1 = sites.dimension(site);
    REQUIRE(loc.x() == loc1.x());
    REQUIRE(loc.y() == loc1.y());
}

namespace {
class SitesWithNamesAndDimensionsFixture{
public:
    Sites sites;
    Site site1, site2, site3;
    std::string name1, name2, name3;
    ophidian::geometry::Location loc1, loc2, loc3;

    SitesWithNamesAndDimensionsFixture() {
        site1 = sites.add();
        site2 = sites.add();
        site3 = sites.add();
        name1 = "core1";
        name2 = "core2";
        name3 = "core3";

        loc1 = ophidian::geometry::Location(ophidian::util::micrometer_t(0.19), ophidian::util::micrometer_t(1.71));
        loc2 = ophidian::geometry::Location(ophidian::util::micrometer_t(0.20), ophidian::util::micrometer_t(1.72));
        loc3 = ophidian::geometry::Location(ophidian::util::micrometer_t(0.21), ophidian::util::micrometer_t(1.73));

        sites.name(site1, name1);
        sites.name(site2, name2);
        sites.name(site3, name3);

        sites.dimension(site1, loc1);
        sites.dimension(site2, loc2);
        sites.dimension(site3, loc3);
    }
};
}

TEST_CASE_METHOD(SitesWithNamesAndDimensionsFixture, "Sites: Check names.", "[floorplan][Sites]")
{
    REQUIRE(sites.sites_range().size() == 3);
    REQUIRE(sites.name(*sites.sites_range().begin()) == name1);
    REQUIRE(sites.name(*(++sites.sites_range().begin())) == name2);
    REQUIRE(sites.name(*(--sites.sites_range().end())) == name3);
}

TEST_CASE_METHOD(SitesWithNamesAndDimensionsFixture, "Sites: Check dimensions.", "[floorplan][Sites]")
{
    auto loc1Ret = sites.dimension(*sites.sites_range().begin());
    REQUIRE(loc1Ret.x() == loc1.x());
    REQUIRE(loc1Ret.y() == loc1.y());

    auto loc2Ret = sites.dimension(*(++sites.sites_range().begin()));
    REQUIRE(loc2Ret.x() == loc2.x());
    REQUIRE(loc2Ret.y() == loc2.y());

    auto loc3Ret = sites.dimension(*(--sites.sites_range().end()));
    REQUIRE(loc3Ret.x() == loc3.x());
    REQUIRE(loc3Ret.y() == loc3.y());
}

TEST_CASE_METHOD(SitesWithNamesAndDimensionsFixture, "Sites: Remove and check properties.", "[floorplan][Sites]")
{
    sites.erase(site1);
    REQUIRE(sites.sites_range().size() == 2);
    auto nameRet1 = sites.name(*sites.sites_range().begin());
    REQUIRE(nameRet1 == name3);
    auto loc1Ret = sites.dimension(*sites.sites_range().begin());
    REQUIRE(loc1Ret.x() == loc3.x());
    REQUIRE(loc1Ret.y() == loc3.y());

    sites.erase(site3);
    REQUIRE(sites.sites_range().size() == 1);
    nameRet1 = sites.name(*sites.sites_range().begin());
    REQUIRE(nameRet1 == name2);
    loc1Ret = sites.dimension(*sites.sites_range().begin());
    REQUIRE(loc1Ret.x() == loc2.x());
    REQUIRE(loc1Ret.y() == loc2.y());
}
