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

TEST_CASE("Sites: Add upperRightCorner property.", "[floorplan][Sites]")
{
    Sites sites;
    auto site = sites.add();

    ophidian::util::Location loc(0.19, 1.71);
    sites.upperRightCorner(site, loc);
    auto loc1 = sites.upperRightCorner(site);
    REQUIRE(loc == loc1);
}

namespace {
class SitesWithPropertiesFixture {
public:
    Sites sites;
    Site site1, site2, site3;
    std::string name1, name2, name3;
    ophidian::util::Location loc1, loc2, loc3;

    SitesWithPropertiesFixture() {
        site1 = sites.add();
        site2 = sites.add();
        site3 = sites.add();
        name1 = "core1";
        name2 = "core2";
        name3 = "core3";

        loc1 = ophidian::util::Location(0.19, 1.71);
        loc2 = ophidian::util::Location(0.20, 1.72);
        loc3 = ophidian::util::Location(0.21, 1.73);

        sites.name(site1, name1);
        sites.name(site2, name2);
        sites.name(site3, name3);

        sites.upperRightCorner(site1, loc1);
        sites.upperRightCorner(site2, loc2);
        sites.upperRightCorner(site3, loc3);
    }
};
}

TEST_CASE_METHOD(SitesWithPropertiesFixture, "Sites: Check names.", "[floorplan][Sites]")
{
    REQUIRE(sites.sites_range().size() == 3);
    REQUIRE(sites.name(*sites.sites_range().begin()) == name1);
    REQUIRE(sites.name(*(++sites.sites_range().begin())) == name2);
    REQUIRE(sites.name(*(--sites.sites_range().end())) == name3);
}

TEST_CASE_METHOD(SitesWithPropertiesFixture, "Sites: Check dimensions.", "[floorplan][Sites]")
{
    auto loc1Ret = sites.upperRightCorner(*sites.sites_range().begin());
    REQUIRE(loc1Ret == loc1);

    auto loc2Ret = sites.upperRightCorner(*(++sites.sites_range().begin()));
    REQUIRE(loc2Ret == loc2);

    auto loc3Ret = sites.upperRightCorner(*(--sites.sites_range().end()));
    REQUIRE(loc3Ret == loc3);
}

TEST_CASE_METHOD(SitesWithPropertiesFixture, "Sites: Remove and check properties.", "[floorplan][Sites]")
{
    sites.erase(site1);
    REQUIRE(sites.sites_range().size() == 2);
    auto nameRet1 = sites.name(*sites.sites_range().begin());
    REQUIRE(nameRet1 == name3);
    auto loc1Ret = sites.upperRightCorner(*sites.sites_range().begin());
    REQUIRE(loc1Ret == loc3);

    sites.erase(site3);
    REQUIRE(sites.sites_range().size() == 1);
    nameRet1 = sites.name(*sites.sites_range().begin());
    REQUIRE(nameRet1 == name2);
    loc1Ret = sites.upperRightCorner(*sites.sites_range().begin());
    REQUIRE(loc1Ret == loc2);
}
