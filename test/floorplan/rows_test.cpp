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
