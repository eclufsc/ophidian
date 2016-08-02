
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

class point_comparator {
    ophidian::geometry::point<double> m_expected_point;
public:
    point_comparator(ophidian::geometry::point<double> expected_point)
        : m_expected_point(expected_point){

    }

    bool operator()(const ophidian::geometry::point<double> & point) {
        return boost::geometry::equals(point, m_expected_point);
    }
};

class cluster_comparator {
    std::vector<ophidian::geometry::point<double>> m_expected_cluster;
public:
    cluster_comparator(std::vector<ophidian::geometry::point<double>> & expected_cluster)
        : m_expected_cluster(expected_cluster){

    }

    bool operator()(const std::vector<ophidian::geometry::point<double>> & cluster) {
        if (cluster.size() != m_expected_cluster.size()) {
            return false;
        }
        for (unsigned flip_flop_index = 0; flip_flop_index < cluster.size(); ++flip_flop_index) {
            if (!boost::geometry::equals(cluster[flip_flop_index], m_expected_cluster[flip_flop_index])) {
                return false;
            }
        }
        return true;
    }
};

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
    ophidian::entity_system::entity_system flip_flop_system;
    std::vector<ophidian::register_clustering::clusters::cluster_element> flip_flops;
    flip_flops.reserve(flip_flop_positions.size());
    for (unsigned flip_flop_index = 0; flip_flop_index < flip_flop_positions.size(); ++flip_flop_index) {
        auto flip_flop = flip_flop_system.create();
        flip_flops.push_back(ophidian::register_clustering::clusters::cluster_element(flip_flop, flip_flop_positions[flip_flop_index]));
    }
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
    ophidian::register_clustering::register_clustering register_clustering;
    ophidian::register_clustering::kmeans<ophidian::register_clustering::initialize_centers_from_vector, ophidian::register_clustering::assign_flip_flop_to_closest_cluster_using_rtree, ophidian::register_clustering::update_center_as_mean> kmeans(register_clustering, initial_centers);
    kmeans.cluster_registers(flip_flops);

    REQUIRE(register_clustering.clusters_system().size() == initial_centers.size());
    for (auto & cluster : register_clustering.clusters_system()) {
        auto cluster_center = register_clustering.cluster_center(cluster);
        auto center_it = std::find_if(expected_centers.begin(), expected_centers.end(), point_comparator(cluster_center));
        REQUIRE(center_it != expected_centers.end());
        expected_centers.erase(center_it);

        auto cluster_flip_flops = register_clustering.cluster_flip_flops(cluster);
        std::vector<ophidian::geometry::point<double>> cluster_positions;
        cluster_positions.reserve(cluster_flip_flops.size());
        for (auto flip_flop : cluster_flip_flops) {
            cluster_positions.push_back(flip_flop.second);
        }
        auto flip_flops_it = std::find_if(expected_clusters.begin(), expected_clusters.end(), cluster_comparator(cluster_positions));
        REQUIRE(flip_flops_it != expected_clusters.end());
        expected_clusters.erase(flip_flops_it);
    }
}

