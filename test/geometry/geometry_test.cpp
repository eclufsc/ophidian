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
