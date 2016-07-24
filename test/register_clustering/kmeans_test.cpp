
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
#include "kmeans.h"

TEST_CASE("kmeans/ clustering 16 registers","[kmeans]") {
    std::vector<ophidian::geometry::point<double>> flip_flop_positions = {
        {1, 1},
        {3, 2},
        {2, 1},
        {1, 3},
        {7, 2},
        {7, 1},
        {9, 1},
        {9, 3},
        {1, 9},
        {3, 7},
        {1, 8},
        {2, 7},
        {7, 7},
        {7, 9},
        {7, 8},
        {8, 7}
    };
    std::vector<ophidian::geometry::point<double>> initial_centers = {
        {2, 2},
        {8, 2},
        {2, 8},
        {8, 8}
    };
    std::vector<ophidian::geometry::point<double>> expected_centers = {
        {1.75, 1.75},
        {8, 1.75},
        {1.75, 7.75},
        {7.25, 7.75}
    };
    std::vector<std::vector<ophidian::geometry::point<double>>> expected_clusters = {
        {{1, 1}, {3, 2}, {2, 1}, {1, 3}},
        {{7, 2}, {7, 1}, {9, 1}, {9, 3}},
        {{1, 9}, {3, 7}, {1, 8}, {2, 7}},
        {{7, 7}, {7, 9}, {7, 8}, {8, 7}}
    };
    ophidian::register_clustering::kmeans<ophidian::register_clustering::initialize_centers_from_vector, ophidian::register_clustering::assign_flip_flop_to_closest_cluster, ophidian::register_clustering::update_center_as_mean> kmeans(initial_centers);
    kmeans.cluster_registers(flip_flop_positions);

    REQUIRE(kmeans.clusters_system().size() == initial_centers.size());
    for (auto & cluster : kmeans.clusters_system()) {
        auto cluster_flip_flops = kmeans.cluster_flip_flops(cluster);
        REQUIRE(cluster_flip_flops.size() == 4);
        std::cout << "cluster" << std::endl;
        for (auto & position : cluster_flip_flops) {
            std::cout << "flip flop " << position.x() << ", " << position.y() << std::endl;
        }
//        auto cluster_center = kmeans.cluster_center(cluster);
//        auto center_it = std::find(expected_centers.begin(), expected_centers.end(), cluster_center);
//        REQUIRE(center_it != expected_centers.end());
//        expected_centers.erase(center_it);
//        auto cluster_flip_flops = kmeans.cluster_flip_flops(cluster);
//        auto flip_flops_it = std::find(expected_clusters.begin(), expected_clusters.end(), cluster_flip_flops);
//        REQUIRE(flip_flops_it != expected_clusters.end());
//        expected_clusters.erase(flip_flops_it);
    }
}

