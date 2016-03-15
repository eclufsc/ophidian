//
// Created by renan on 3/10/16.
//

#include "../catch.hpp"
#include "../interconnection/hpwl.h"

TEST_CASE("interconnection_estimation/hpwl test", "[interconnection][hpwl]") {
    std::vector< openeda::geometry::point<double> > points
    {
        openeda::geometry::point<double>{80, 50},
        openeda::geometry::point<double>{140, 250},
        openeda::geometry::point<double>{405, 40},
        openeda::geometry::point<double>{207, 445}
    };

    double hpwl = openeda::interconnection::hpwl(points);
    REQUIRE( hpwl == 730.0 );
}

