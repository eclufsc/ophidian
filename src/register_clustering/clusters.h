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

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include "../geometry/geometry.h"
#include "entity_system.h"
#include "vector_property.h"

namespace ophidian {
namespace register_clustering {
class clusters
{
public:
    using point = geometry::point<double>;
private:
    entity_system::entity_system & m_system;

    entity_system::vector_property<std::vector<point>> m_flip_flops;
    entity_system::vector_property<point> m_centers;
public:
    clusters(entity_system::entity_system & system);

    const std::vector<point> & flip_flops(entity_system::entity cluster) const {
        return m_flip_flops[m_system.lookup(cluster)];
    }

    void insert_flip_flop(entity_system::entity cluster, point flip_flop);
    void remove_flip_flops(entity_system::entity cluster);

    point center(entity_system::entity cluster) const {
        return m_centers[m_system.lookup(cluster)];
    }

    void center(entity_system::entity cluster, point center);
};
}
}

#endif // CLUSTERS_H
