#include <catch.hpp>
#include <ophidian/floorplan/Floorplan.h>
#include <ophidian/util/Units.h>

using namespace ophidian::floorplan;
using dbu_t = ophidian::util::database_unit_t;

TEST_CASE("Floorplan: Brand-new floorplan must be empty.", "[floorplan]")
{
	auto origin = Floorplan::point_type{dbu_t{0.0}, dbu_t{0.0}};
	auto boundaries = Floorplan::point_type{dbu_t{0.0}, dbu_t{0.0}};

	Floorplan floorplan;
	CHECK(floorplan.chip_origin().x() == origin.x());
	CHECK(floorplan.chip_origin().y() == origin.y());

	CHECK(floorplan.chip_upper_right_corner().x() == boundaries.x());
	CHECK(floorplan.chip_upper_right_corner().y() == boundaries.y());

	CHECK(floorplan.range_site().empty());
	CHECK(floorplan.range_row().empty());
}

TEST_CASE("Floorplan: Set Origing and boundaries.", "[floorplan]")
{
	auto origin = Floorplan::point_type{dbu_t{2.0}, dbu_t{3.0}};
	auto boundaries = Floorplan::point_type{dbu_t{4000.0}, dbu_t{3000.0}};

	Floorplan floorplan;
	floorplan.chip_origin() = origin;
	floorplan.chip_upper_right_corner() = boundaries;

	CHECK(floorplan.chip_origin().x() == origin.x());
	CHECK(floorplan.chip_origin().y() == origin.y());
    
	CHECK(floorplan.chip_upper_right_corner().x() == boundaries.x());
	CHECK(floorplan.chip_upper_right_corner().y() == boundaries.y());
}

namespace
{
    class SitesWithPropertiesFixture
    {
    public:
        std::string name1, name2, name3;
        Floorplan::point_type loc1, loc2, loc3;

        SitesWithPropertiesFixture() {
            name1 = "core1";
            name2 = "core2";
            name3 = "core3";

            loc1 = Floorplan::point_type{dbu_t{0.19}, dbu_t{1.71}};
            loc2 = Floorplan::point_type{dbu_t{0.20}, dbu_t{1.72}};
            loc3 = Floorplan::point_type{dbu_t{0.21}, dbu_t{1.73}};
        }
    };
}

TEST_CASE_METHOD(SitesWithPropertiesFixture,"Floorplan: Add/Erase Sites.", "[floorplan]")
{
    Floorplan floorplan;
    auto siteRet1 = floorplan.add_site(name1, loc1);
    auto siteRet2 = floorplan.add_site(name2, loc2);
    auto siteRet3 = floorplan.add_site(name3, loc3);

    CHECK(floorplan.range_site().size() == 3);
    CHECK(floorplan.name(siteRet1) == name1);
    CHECK(floorplan.name(siteRet2) == name2);
    CHECK(floorplan.name(siteRet3) == name3);

    CHECK(floorplan.dimension(siteRet1).x() == loc1.x());
    CHECK(floorplan.dimension(siteRet1).y() == loc1.y());

    CHECK(floorplan.dimension(siteRet2).x() == loc2.x());
    CHECK(floorplan.dimension(siteRet2).y() == loc2.y());

    CHECK(floorplan.dimension(siteRet3).x() == loc3.x());
    CHECK(floorplan.dimension(siteRet3).y() == loc3.y());

    floorplan.erase(siteRet1);
    floorplan.erase(siteRet2);
    floorplan.erase(siteRet3);
    CHECK(floorplan.range_site().empty());
}

TEST_CASE_METHOD(SitesWithPropertiesFixture,"Floorplan: Test site mapping.", "[floorplan]")
{
    Floorplan floorplan;
    floorplan.add_site(name1, loc1);
    auto site1 = floorplan.find(name1);
    CHECK(floorplan.range_site().size() == 1);
    floorplan.erase(site1);
    CHECK(floorplan.range_site().size() == 0);
}

namespace
{
    class RowWithPropertiesFixture
    {
    public:
        Floorplan::point_type origin1, origin2, origin3;
        Floorplan::row_size_type numSites1, numSites2, numSites3;

        SitesWithPropertiesFixture sitesSystem;

        RowWithPropertiesFixture() {
            origin1 = Floorplan::point_type{dbu_t{10.0}, dbu_t{10.0}};
            origin2 = Floorplan::point_type{dbu_t{200.0}, dbu_t{200.0}};
            origin3 = Floorplan::point_type{dbu_t{400.0}, dbu_t{400.0}};

            numSites1 = 100;
            numSites2 = 150;
            numSites3 = 200;
        }
    };
}

TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Add/Erase Rows.", "[floorplan]")
{
    Floorplan floorplan;
    auto site1 = floorplan.add_site(sitesSystem.name1, sitesSystem.loc1);
    auto site2 = floorplan.add_site(sitesSystem.name2, sitesSystem.loc2);
    auto site3 = floorplan.add_site(sitesSystem.name3, sitesSystem.loc3);

    auto rowRet1 = floorplan.add_row(origin1, numSites1, site1);
    auto rowRet2 = floorplan.add_row(origin2, numSites2, site2);
    auto rowRet3 = floorplan.add_row(origin3, numSites3, site3);

    CHECK(floorplan.range_row().size() == 3);

    CHECK(floorplan.origin(rowRet1).x() == origin1.x());
    CHECK(floorplan.origin(rowRet1).y() == origin1.y());

    CHECK(floorplan.origin(rowRet2).x() == origin2.x());
    CHECK(floorplan.origin(rowRet2).y() == origin2.y());

    CHECK(floorplan.origin(rowRet3).x() == origin3.x());
    CHECK(floorplan.origin(rowRet3).y() == origin3.y());

    CHECK(floorplan.number_of_sites(rowRet1) == numSites1);
    CHECK(floorplan.number_of_sites(rowRet2) == numSites2);
    CHECK(floorplan.number_of_sites(rowRet3) == numSites3);

    CHECK(floorplan.site(rowRet1) == site1);
    CHECK(floorplan.site(rowRet2) == site2);
    CHECK(floorplan.site(rowRet3) == site3);

    floorplan.erase(rowRet1);
    floorplan.erase(rowRet2);
    floorplan.erase(rowRet3);
    CHECK(floorplan.range_row().empty());
}

TEST_CASE_METHOD(RowWithPropertiesFixture,"Floorplan: Row Upper Right Corner.", "[floorplan]")
{
    Floorplan floorplan;
    auto siteRet1 = floorplan.add_site(sitesSystem.name1, sitesSystem.loc1);
    auto siteRet2 = floorplan.add_site(sitesSystem.name2, sitesSystem.loc2);
    auto rowRet1 = floorplan.add_row(origin1, numSites1, siteRet1);
    auto rowRet2 = floorplan.add_row(origin2, numSites2, siteRet2);

    auto urCorner1 = Floorplan::point_type{dbu_t{29.0}, dbu_t{11.71}};

    CHECK(floorplan.upper_right_corner(rowRet1).x() == urCorner1.x());
    CHECK(floorplan.upper_right_corner(rowRet1).y() == urCorner1.y());

    auto urCorner2 = Floorplan::point_type{dbu_t{230.0}, dbu_t{201.72}};

    CHECK(floorplan.upper_right_corner(rowRet2).x() == urCorner2.x());
    CHECK(floorplan.upper_right_corner(rowRet2).y() == urCorner2.y());
}
