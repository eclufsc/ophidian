//
// Created by renan on 2/9/16.
//

#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"
#include "sites.h"
#include "rows.h"

TEST_CASE("sites/assign site", "[floorplan][rows]") {
    openeda::entity::system sites_system;
    openeda::floorplan::sites sites(sites_system);
    openeda::entity::system rows_system;
    openeda::floorplan::rows rows(rows_system);

    auto core_site = sites_system.create();
    auto row = rows_system.create();
    rows.site(row, core_site);

    REQUIRE(rows.site(row) == core_site);
}

TEST_CASE("sites/assign number of sites", "[floorplan][rows]") {
    openeda::entity::system rows_system;
    openeda::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    unsigned number_of_sites = 100;
    rows.number_of_sites(row, number_of_sites);

    REQUIRE(rows.number_of_sites(row) == number_of_sites);
}

TEST_CASE("sites/assign origin", "[floorplan][rows]") {
    openeda::entity::system rows_system;
    openeda::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    openeda::geometry::point<double> origin(0, 0);
    rows.origin(row, origin);

    REQUIRE(boost::geometry::equals(rows.origin(row), origin));
}

TEST_CASE("sites/read property from removed row", "[floorplan][rows]") {
    openeda::entity::system rows_system;
    openeda::floorplan::rows rows(rows_system);

    auto row = rows_system.create();
    unsigned number_of_sites = 100;
    rows.number_of_sites(row, number_of_sites);
    rows_system.destroy(row);

    REQUIRE_THROWS(rows.number_of_sites(row));
}