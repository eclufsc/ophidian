//
// Created by renan on 2/9/16.
//

#include <floorplan.h>
#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"

TEST_CASE("floorplan/ empty","[floorplan]") {
    openeda::geometry::point<double> chip_boundaries(1000, 1000);
    openeda::floorplan::floorplan floorplan(chip_boundaries);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE(floorplan.row_count() == 0);
    REQUIRE(boost::geometry::equals(floorplan.chip_boundaries(), chip_boundaries));
}

TEST_CASE("floorplan/ insert site","[floorplan]") {
    openeda::geometry::point<double> chip_boundaries(1000, 1000);
    openeda::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    openeda::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    REQUIRE(floorplan.site_count() == 1);
    REQUIRE(floorplan.site_name(site) == site_name);
    REQUIRE(boost::geometry::equals(floorplan.site_dimensions(site), site_dimensions));
}

TEST_CASE("floorplan/ insert row","[floorplan]") {
    openeda::geometry::point<double> chip_boundaries(1000, 1000);
    openeda::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    openeda::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    unsigned number_of_sites = 100;
    openeda::geometry::point<double> row_origin(0, 0);
    auto row = floorplan.row_insert(site, number_of_sites, row_origin);

    REQUIRE(floorplan.row_count() == 1);
    REQUIRE(floorplan.row_site(row) == site);
    REQUIRE(floorplan.row_number_of_sites(row) == number_of_sites);
    REQUIRE(boost::geometry::equals(floorplan.row_origin(row), row_origin));

    openeda::geometry::point<double> expected_row_dimensions(10*100, 100);
    REQUIRE(boost::geometry::equals(floorplan.row_dimensions(row), expected_row_dimensions));
}

TEST_CASE("floorplan/ destroy site","[floorplan]") {
    openeda::geometry::point<double> chip_boundaries(1000, 1000);
    openeda::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    openeda::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    floorplan.site_destroy(site);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE_THROWS(floorplan.site_name(site));
    REQUIRE_THROWS(floorplan.site_dimensions(site));
}

TEST_CASE("floorplan/ destroy row","[floorplan]") {
    openeda::geometry::point<double> chip_boundaries(1000, 1000);
    openeda::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    openeda::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    unsigned number_of_sites = 100;
    openeda::geometry::point<double> row_origin(0, 0);
    auto row = floorplan.row_insert(site, number_of_sites, row_origin);

    floorplan.row_destroy(row);

    REQUIRE(floorplan.row_count() == 0);
    REQUIRE_THROWS(floorplan.row_site(row));
    REQUIRE_THROWS(floorplan.row_number_of_sites(row));
    REQUIRE_THROWS(floorplan.row_origin(row));
}