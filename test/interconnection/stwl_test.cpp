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

#include "../catch.hpp"
#include "../interconnection/hpwl.h"
#include "../interconnection/stwl.h"

TEST_CASE("interconnection_estimation/stwl test with two points", "[interconnection][stwl]") {
    std::vector< ophidian::geometry::point<double> > points{
        ophidian::geometry::point<double>{80, 50},
        ophidian::geometry::point<double>{140, 260}
    };
    double StWL{ophidian::interconnection::stwl(points)};
    double HPWL{ophidian::interconnection::hpwl(points)};
    REQUIRE( StWL == HPWL );
}

TEST_CASE("interconnection_estimation/stwl test with three points", "[interconnection][stwl]") {
    std::vector< ophidian::geometry::point<double> > points{
        ophidian::geometry::point<double>{80, 50},
        ophidian::geometry::point<double>{140, 260},
        ophidian::geometry::point<double>{405, 40}
    };
    double StWL{ophidian::interconnection::stwl(points)};
    double HPWL{ophidian::interconnection::hpwl(points)};
    REQUIRE( StWL == HPWL );
}

TEST_CASE("interconnection_estimation/stwl test with four points", "[interconnection][stwl]") {
    std::vector< ophidian::geometry::point<double> > points{
        ophidian::geometry::point<double>{80, 50},
        ophidian::geometry::point<double>{140, 260},
        ophidian::geometry::point<double>{405, 40},
        ophidian::geometry::point<double>{207, 445}
    };
    double StWL{ophidian::interconnection::stwl(points)};
    REQUIRE( StWL == 797.0 );
}
