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

#include "../geometry/geometry.h"
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

TEST_CASE("geometry/distance between points", "[geometry][distance]") {

	ophidian::geometry::point<double> point1(0.0, 0.0);
	ophidian::geometry::point<double> point2(std::sqrt(2.0), std::sqrt(2.0));

	auto distance = boost::geometry::distance(point1, point2);
	REQUIRE(distance == 2.0);

}

TEST_CASE("geometry/manhattan distance", "[geometry][distance][manhattan]") {
	ophidian::geometry::point<double> point1(0.0, 0.0);
	ophidian::geometry::point<double> point2(1.0, 1.0);
	auto distance = ophidian::geometry::manhattan_distance(point1, point2);
	REQUIRE(distance == 2.0);
}
