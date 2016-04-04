/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <floorplan.h>
#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"

TEST_CASE("floorplan/ empty","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan(chip_boundaries);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE(floorplan.row_count() == 0);
    REQUIRE(boost::geometry::equals(floorplan.chip_boundaries(), chip_boundaries));
}

TEST_CASE("floorplan/ insert site","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    REQUIRE(floorplan.site_count() == 1);
    REQUIRE(floorplan.site_name(site) == site_name);
    REQUIRE(boost::geometry::equals(floorplan.site_dimensions(site), site_dimensions));
}

TEST_CASE("floorplan/ insert row","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    unsigned number_of_sites = 100;
    ophidian::geometry::point<double> row_origin(0, 0);
    auto row = floorplan.row_insert(site, number_of_sites, row_origin);

    REQUIRE(floorplan.row_count() == 1);
    REQUIRE(floorplan.row_site(row) == site);
    REQUIRE(floorplan.row_number_of_sites(row) == number_of_sites);
    REQUIRE(boost::geometry::equals(floorplan.row_origin(row), row_origin));

    ophidian::geometry::point<double> expected_row_dimensions(10*100, 100);
    REQUIRE(boost::geometry::equals(floorplan.row_dimensions(row), expected_row_dimensions));
}

TEST_CASE("floorplan/ destroy site","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    floorplan.site_destroy(site);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE_THROWS(floorplan.site_name(site));
    REQUIRE_THROWS(floorplan.site_dimensions(site));
}

TEST_CASE("floorplan/ destroy row","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan(chip_boundaries);

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    unsigned number_of_sites = 100;
    ophidian::geometry::point<double> row_origin(0, 0);
    auto row = floorplan.row_insert(site, number_of_sites, row_origin);

    floorplan.row_destroy(row);

    REQUIRE(floorplan.row_count() == 0);
    REQUIRE_THROWS(floorplan.row_site(row));
    REQUIRE_THROWS(floorplan.row_number_of_sites(row));
    REQUIRE_THROWS(floorplan.row_origin(row));
}
