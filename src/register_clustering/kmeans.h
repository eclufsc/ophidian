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

#ifndef KMEANS_H
#define KMEANS_H

#include <boost/geometry/index/rtree.hpp>
#include "../geometry/geometry.h"
#include "clusters.h"
#include "register_clustering.h"

namespace ophidian {
namespace register_clustering {

class initialize_centers_from_vector {
public:
    using point = geometry::point<double>;
private:
    const std::vector<point> & m_initial_centers;
public:
    initialize_centers_from_vector(const std::vector<point> & initial_centers)
        : m_initial_centers(initial_centers) {

    }

    void initialize_centers(register_clustering & register_clustering);
};

class assign_flip_flop_to_closest_cluster {
public:
    using point = geometry::point<double>;

    assign_flip_flop_to_closest_cluster()
    {

    }

    void assign_flops_to_clusters(const std::vector<clusters::cluster_element> & flip_flops, register_clustering & register_clustering);
};

class assign_flip_flop_to_closest_cluster_using_rtree {
public:
    using point = geometry::point<double>;
    using rtree_node = std::pair<point, entity_system::entity>;
    using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

    assign_flip_flop_to_closest_cluster_using_rtree()
    {

    }

    void assign_flops_to_clusters(const std::vector<clusters::cluster_element> & flip_flops, register_clustering & register_clustering);
};

class update_center_as_mean {
public:
    using point = geometry::point<double>;

    update_center_as_mean()
    {

    }

    void update_cluster_centers(register_clustering & register_clustering);
};

template <class InitializationStrategy = initialize_centers_from_vector, class AssignmentStrategy = assign_flip_flop_to_closest_cluster, class CenterUpdateStrategy = update_center_as_mean>
class kmeans
{
public:
    using point = geometry::point<double>;    
private:
    register_clustering & m_register_clustering;

    InitializationStrategy m_initialization_strategy;
    AssignmentStrategy m_assignment_strategy;
    CenterUpdateStrategy m_center_update_strategy;
public:
    kmeans(ophidian::register_clustering::register_clustering & register_clustering, InitializationStrategy & initialization_strategy, AssignmentStrategy & assignment_strategy, CenterUpdateStrategy & center_update_strategy)
        : m_register_clustering(register_clustering), m_initialization_strategy(initialization_strategy), m_assignment_strategy(assignment_strategy), m_center_update_strategy(center_update_strategy) {
        initialize_centers();
    }

    kmeans(ophidian::register_clustering::register_clustering & register_clustering, const std::vector<point> & initial_centers)
        : m_register_clustering(register_clustering), m_initialization_strategy(initial_centers) {
        initialize_centers();
    }

    void initialize_centers() {
        m_initialization_strategy.initialize_centers(m_register_clustering);
    }

    void cluster_registers(const std::vector<clusters::cluster_element> & flip_flops, unsigned iterations = 1) {
        for (unsigned iteration = 0; iteration < iterations; ++iteration) {
            m_assignment_strategy.assign_flops_to_clusters(flip_flops, m_register_clustering);
            m_center_update_strategy.update_cluster_centers(m_register_clustering);
        }
    }
};
}
}

#endif // KMEANS_H
