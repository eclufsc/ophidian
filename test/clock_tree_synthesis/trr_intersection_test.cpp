

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
#include "deffered_merge_embedding.h"

//TEST_CASE("trr intersection/ edge intersection of two squared trrs","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {1.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({3.0, 3.0}, {3.0, 3.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 2.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 1.0}, {1.0, 3.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ point intersection of two squared trrs","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {1.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({5.0, 1.0}, {5.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 2.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 1.0}, {3.0, 1.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ edge intersection of two rectangular trrs","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {2.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 1.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({3.0, 3.0}, {4.0, 4.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 1.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 2.0}, {2.0, 3.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ point intersection of two rectangular trrs","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {2.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 1.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({4.0, 2.0}, {5.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 1.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 2.0}, {3.0, 2.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ edge intersection between segment and squared trr","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {1.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({2.0, 1.0}, {4.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 0.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({2.0, 1.0}, {3.0, 1.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ point intersection between segment and squared trr","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {1.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({3.0, 1.0}, {4.0, 1.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 0.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 1.0}, {3.0, 1.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ edge intersection between segment and rectangular trr","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {2.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({3.0, 2.0}, {5.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 0.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({3.0, 2.0}, {4.0, 2.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}

//TEST_CASE("trr intersection/ point intersection between segment and rectangular trr","[trr]") {
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr1({1.0, 1.0}, {2.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr1(core_trr1, 2.0);
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment core_trr2({4.0, 2.0}, {5.0, 2.0});
//    ophidian::clock_tree_synthesis::deffered_merge_embedding::trr trr2(core_trr2, 0.0);

//    ophidian::clock_tree_synthesis::deffered_merge_embedding::segment expected_intersection({4.0, 2.0}, {4.0, 2.0});
//    auto intersection = trr1.intersection(trr2);
//    std::cout << intersection.first.x() << ", " << intersection.first.y() << " -> " << intersection.second.x() << ", " << intersection.second.y() << std::endl;
//    REQUIRE(boost::geometry::equals(expected_intersection, intersection));
//}
