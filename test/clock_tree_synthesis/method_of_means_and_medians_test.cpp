
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
#include "method_of_means_and_medians.h"

TEST_CASE("method of means and medians/ build topology of network with 8 flip flops","[mmm]") {
    std::vector<ophidian::geometry::point<double>> flip_flop_positions = {
        {1, 1},
        {3, 2},
        {7, 2},
        {7, 1},
        {1, 9},
        {3, 7},
        {7, 7},
        {7, 9}
    };
    ophidian::geometry::point<double> clock_source(5, 0);

    ophidian::clock_tree_synthesis::method_of_means_and_medians mmm;
    ophidian::clock_tree_synthesis::clock_topology clock_topology;
    mmm.build_topology(clock_source, flip_flop_positions, clock_topology);

    std::vector<ophidian::geometry::point<double>> expected_node_positions = {
        {1, 1},
        {3, 2},
        {7, 2},
        {7, 1},
        {9, 3},
        {3, 7},
        {7, 7},
        {7, 9},
        {5, 0},
        {4.5, 4},
        {2, 4.75},
        {7, 4.75},
        {2, 1.5},
        {7, 1.5},
        {2, 8},
        {7, 8}
    };
    auto & graph = clock_topology.graph();
    for (auto node_it = lemon::ListDigraph::NodeIt(graph); node_it != lemon::INVALID; ++node_it) {
        auto node_position = clock_topology.node_position(node_it);
        std::cout << "node position " << node_position.x() << ", " << node_position.y() << std::endl;
        bool found = false;
        for (auto position : expected_node_positions) {
            if (boost::geometry::equals(position, node_position)) {
                found = true;
            }
        }
        REQUIRE(found);
    }

}


