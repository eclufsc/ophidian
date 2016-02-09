//
// Created by renan on 2/9/16.
//

#include <boost/geometry/algorithms/equals.hpp>
#include "../catch.hpp"
#include "sites.h"

TEST_CASE("sites/assign name", "[floorplan][sites]") {
    openeda::entity::system sites_system;
    openeda::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    std::string name = "core";
    sites.name(core_site, name);
    REQUIRE(sites.name(core_site) == name);
}

TEST_CASE("sites/assign dimensions", "[floorplan][sites]") {
    openeda::entity::system sites_system;
    openeda::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    openeda::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);
    REQUIRE(boost::geometry::equals(sites.dimensions(core_site), site_dimensions));
}

TEST_CASE("sites/read property from removed site", "[floorplan][sites]") {
    openeda::entity::system sites_system;
    openeda::floorplan::sites sites(sites_system);

    auto core_site = sites_system.create();
    openeda::geometry::point<double> site_dimensions(10, 100);
    sites.dimensions(core_site, site_dimensions);

    sites_system.destroy(core_site);
    REQUIRE_THROWS(sites.dimensions(core_site));
}