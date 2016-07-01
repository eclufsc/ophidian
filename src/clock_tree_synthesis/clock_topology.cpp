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

#include "clock_topology.h"

namespace ophidian {
namespace clock_tree_synthesis {
clock_topology::clock_topology()
    : m_positions(m_graph){

}

clock_topology::~clock_topology()
{

}

clock_topology::node clock_topology::node_create(clock_topology::point position)
{
    auto node = m_graph.addNode();
    m_positions[node] = position;
    return node;
}

clock_topology::edge clock_topology::edge_create(clock_topology::node source, clock_topology::node target)
{
    auto edge = m_graph.addArc(source, target);
    return edge;
}
}

}
