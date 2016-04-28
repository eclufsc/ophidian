/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include <floorplan.h>
#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"

TEST_CASE("floorplan/ empty","[floorplan]") {
    ophidian::geometry::point<double> chip_boundaries(1000, 1000);
    ophidian::floorplan::floorplan floorplan;
    floorplan.chip_boundaries(chip_boundaries);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE(floorplan.row_count() == 0);
    REQUIRE(boost::geometry::equals(floorplan.chip_boundaries(), chip_boundaries));
}

TEST_CASE("floorplan/ insert site","[floorplan]") {
    ophidian::floorplan::floorplan floorplan;

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    REQUIRE(floorplan.site_count() == 1);
    REQUIRE(floorplan.site_name(site) == site_name);
    REQUIRE(boost::geometry::equals(floorplan.site_dimensions(site), site_dimensions));
}

TEST_CASE("floorplan/ insert row","[floorplan]") {
    ophidian::floorplan::floorplan floorplan;

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
    ophidian::floorplan::floorplan floorplan;

    std::string site_name = "core";
    ophidian::geometry::point<double> site_dimensions(10, 100);
    auto site = floorplan.site_insert(site_name, site_dimensions);

    floorplan.site_destroy(site);

    REQUIRE(floorplan.site_count() == 0);
    REQUIRE_THROWS(floorplan.site_name(site));
    REQUIRE_THROWS(floorplan.site_dimensions(site));
}

TEST_CASE("floorplan/ destroy row","[floorplan]") {
    ophidian::floorplan::floorplan floorplan;

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
