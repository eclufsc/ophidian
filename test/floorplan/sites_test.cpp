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

TEST_CASE("sites/assign name", "[floorplan][sites]") {
    ophidian::entity_system::entity_system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    std::string name = "core";
    sites.name(core_site, name);
    REQUIRE(sites.name(core_site) == name);
}

TEST_CASE("sites/assign dimensions", "[floorplan][sites]") {
    ophidian::entity_system::entity_system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    ophidian::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);
    REQUIRE(boost::geometry::equals(sites.dimensions(core_site), site_dimensions));
}

TEST_CASE("sites/read property from removed site", "[floorplan][sites]") {
    ophidian::entity_system::entity_system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    ophidian::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);

    sites_system.destroy(core_site);
    REQUIRE_THROWS(sites.dimensions(core_site));
}
