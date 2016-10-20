#include <catch.hpp>
#include <ophidian/floorplan/Rows.h>

using namespace ophidian::floorplan;

TEST_CASE("Rows: Brand-new rows must be empty.", "[floorplan][Rows]")
{
    Rows rows;
    REQUIRE(rows.rows_range().empty());
}

TEST_CASE("Rows: Add Row.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();
    REQUIRE(rows.rows_range().size() == 1);
}

TEST_CASE("Rows: Erase Row.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();
    rows.erase(row);
    REQUIRE(rows.rows_range().size() == 0);
}

TEST_CASE("Sites: Add four Rows.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();
    row = rows.add();
    row = rows.add();
    row = rows.add();
    REQUIRE(rows.rows_range().size() == 4);
}

TEST_CASE("Rows: Add origin property.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();

    ophidian::geometry::Location origin(ophidian::util::micrometer_t(2.0), ophidian::util::micrometer_t(2.0));
    rows.origin(row, origin);
    auto originReturn = rows.origin(row);
    REQUIRE(originReturn.x() == origin.y());
    REQUIRE(originReturn.y() == origin.y());
}

TEST_CASE("Rows: Add numberOfSites property.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();

    size_t numberOfSites = 100;
    rows.numberOfSites(row, numberOfSites);
    auto numberOfSitesReturn = rows.numberOfSites(row);
    REQUIRE(numberOfSitesReturn == numberOfSites);
}

namespace {
class SitesSystemFixture{
public:
    Sites sites;
    Site site1, site2, site3;

    SitesSystemFixture(){
        site1 = sites.add();
        site2 = sites.add();
        site3 = sites.add();
    }

};
}

TEST_CASE_METHOD(SitesSystemFixture, "Rows: Add siteType property.", "[floorplan][Rows]")
{
    Rows rows;
    auto row = rows.add();

    rows.site(row, site1);
    auto siteReturn = rows.site(row);
    REQUIRE(siteReturn == site1);
}

namespace {
class RowWithPropertiesFixture{
public:
    Rows rows;
    Row row1, row2, row3;
    ophidian::geometry::Location origin1, origin2, origin3;
    size_t numSites1, numSites2, numSites3;

    SitesSystemFixture sitesSystem;

    RowWithPropertiesFixture() {
        row1 = rows.add();
        row2 = rows.add();
        row3 = rows.add();

        origin1 = ophidian::geometry::Location(ophidian::util::micrometer_t(10.0), ophidian::util::micrometer_t(10.0));
        origin2 = ophidian::geometry::Location(ophidian::util::micrometer_t(200.0), ophidian::util::micrometer_t(200.0));
        origin3 = ophidian::geometry::Location(ophidian::util::micrometer_t(400.0), ophidian::util::micrometer_t(400.0));

        numSites1 = 100;
        numSites2 = 150;
        numSites3 = 200;

        rows.origin(row1, origin1);
        rows.origin(row2, origin2);
        rows.origin(row3, origin3);

        rows.numberOfSites(row1, numSites1);
        rows.numberOfSites(row2, numSites2);
        rows.numberOfSites(row3, numSites3);

        rows.site(row1, sitesSystem.site1);
        rows.site(row2, sitesSystem.site2);
        rows.site(row3, sitesSystem.site3);
    }
};
}

TEST_CASE_METHOD(RowWithPropertiesFixture, "Rows: Check row origins.", "[floorplan][Rows]")
{
    REQUIRE(rows.rows_range().size() == 3);
    auto loc1Ret = rows.origin(*rows.rows_range().begin());
    REQUIRE(loc1Ret.x() == origin1.x());
    REQUIRE(loc1Ret.y() == origin1.y());

    auto loc2Ret = rows.origin(*(++rows.rows_range().begin()));
    REQUIRE(loc2Ret.x() == origin2.x());
    REQUIRE(loc2Ret.y() == origin2.y());

    auto loc3Ret = rows.origin(*(--rows.rows_range().end()));
    REQUIRE(loc3Ret.x() == origin3.x());
    REQUIRE(loc3Ret.y() == origin3.y());
}

TEST_CASE_METHOD(RowWithPropertiesFixture, "Rows: Check row sizes.", "[floorplan][Rows]")
{
    REQUIRE(rows.numberOfSites(*rows.rows_range().begin()) == numSites1);
    REQUIRE(rows.numberOfSites(*(++rows.rows_range().begin())) == numSites2);
    REQUIRE(rows.numberOfSites(*(--rows.rows_range().end())) == numSites3);
}

TEST_CASE_METHOD(RowWithPropertiesFixture, "Rows: Check row site types.", "[floorplan][Rows]")
{
    REQUIRE(rows.site(*rows.rows_range().begin()) == sitesSystem.site1);
    REQUIRE(rows.site(*(++rows.rows_range().begin())) == sitesSystem.site2);
    REQUIRE(rows.site(*(--rows.rows_range().end())) == sitesSystem.site3);
}

TEST_CASE_METHOD(RowWithPropertiesFixture, "Rows: Remove and check properties.", "[floorplan][Rows]")
{
    rows.erase(row1);
    REQUIRE(rows.rows_range().size() == 2);
    auto loc1Ret = rows.origin(*rows.rows_range().begin());
    REQUIRE(loc1Ret.x() == origin3.x());
    REQUIRE(loc1Ret.y() == origin3.y());
    auto numSitesRet1 = rows.numberOfSites(*rows.rows_range().begin());
    REQUIRE(numSitesRet1 == numSites3);
    auto siteTypeRet1 = rows.site(*rows.rows_range().begin());
    REQUIRE(siteTypeRet1 == sitesSystem.site3);

    rows.erase(row3);
    REQUIRE(rows.rows_range().size() == 1);
    loc1Ret = rows.origin(*rows.rows_range().begin());
    REQUIRE(loc1Ret.x() == origin2.x());
    REQUIRE(loc1Ret.y() == origin2.y());
    numSitesRet1 = rows.numberOfSites(*rows.rows_range().begin());
    REQUIRE(numSitesRet1 == numSites2);
    siteTypeRet1 = rows.site(*rows.rows_range().begin());
    REQUIRE(siteTypeRet1 == sitesSystem.site2);
}
