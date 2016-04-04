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

#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"
#include "sites.h"

TEST_CASE("sites/assign name", "[floorplan][sites]") {
    ophidian::entity::system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    std::string name = "core";
    sites.name(core_site, name);
    REQUIRE(sites.name(core_site) == name);
}

TEST_CASE("sites/assign dimensions", "[floorplan][sites]") {
    ophidian::entity::system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    ophidian::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);
    REQUIRE(boost::geometry::equals(sites.dimensions(core_site), site_dimensions));
}

TEST_CASE("sites/read property from removed site", "[floorplan][sites]") {
    ophidian::entity::system sites_system;
    ophidian::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    ophidian::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);

    sites_system.destroy(core_site);
    REQUIRE_THROWS(sites.dimensions(core_site));
}
