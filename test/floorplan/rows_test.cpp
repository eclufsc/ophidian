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

#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"
#include "sites.h"
#include "rows.h"

TEST_CASE("sites/assign site", "[floorplan][rows]") {
    ophidian::entity::system sites_system;
    ophidian::floorplan::sites sites(sites_system);
    ophidian::entity::system rows_system;
    ophidian::floorplan::rows rows(rows_system);

    auto core_site = sites_system.create();
    auto row = rows_system.create();
    rows.site(row, core_site);

    REQUIRE(rows.site(row) == core_site);
}

TEST_CASE("sites/assign number of sites", "[floorplan][rows]") {
    ophidian::entity::system rows_system;
    ophidian::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    unsigned number_of_sites = 100;
    rows.number_of_sites(row, number_of_sites);

    REQUIRE(rows.number_of_sites(row) == number_of_sites);
}

TEST_CASE("sites/assign origin", "[floorplan][rows]") {
    ophidian::entity::system rows_system;
    ophidian::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    ophidian::geometry::point<double> origin(0, 0);
    rows.origin(row, origin);

    REQUIRE(boost::geometry::equals(rows.origin(row), origin));
}

TEST_CASE("sites/read property from removed row", "[floorplan][rows]") {
    ophidian::entity::system rows_system;
    ophidian::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    unsigned number_of_sites = 100;
    rows.number_of_sites(row, number_of_sites);
    rows_system.destroy(row);

    REQUIRE_THROWS(rows.number_of_sites(row));
}
