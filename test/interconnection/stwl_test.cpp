//
// Created by renan on 3/10/16.
//

#include "../catch.hpp"
#include "../interconnection/hpwl.h"
#include "../interconnection/stwl.h"

TEST_CASE("interconnection_estimation/stwl test with two points", "[interconnection][stwl]") {
    std::vector< openeda::geometry::point<double> > points{
        openeda::geometry::point<double>{80, 50},
        openeda::geometry::point<double>{140, 260}
    };
    double StWL{openeda::interconnection::stwl(points)};
    double HPWL{openeda::interconnection::hpwl(points)};
    REQUIRE( StWL == HPWL );
}

TEST_CASE("interconnection_estimation/stwl test with three points", "[interconnection][stwl]") {
    std::vector< openeda::geometry::point<double> > points{
        openeda::geometry::point<double>{80, 50},
        openeda::geometry::point<double>{140, 260},
        openeda::geometry::point<double>{405, 40}
    };
    double StWL{openeda::interconnection::stwl(points)};
    double HPWL{openeda::interconnection::hpwl(points)};
    REQUIRE( StWL == HPWL );
}

TEST_CASE("interconnection_estimation/stwl test with four points", "[interconnection][stwl]") {
    std::vector< openeda::geometry::point<double> > points{
        openeda::geometry::point<double>{80, 50},
        openeda::geometry::point<double>{140, 260},
        openeda::geometry::point<double>{405, 40},
        openeda::geometry::point<double>{207, 445}
    };
    double StWL{openeda::interconnection::stwl(points)};
    REQUIRE( StWL == 797.0 );
}
