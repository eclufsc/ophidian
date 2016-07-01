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

#ifndef CLOCK_TREE_SYNTHESIS_H
#define CLOCK_TREE_SYNTHESIS_H

#include "flip_flops.h"

namespace ophidian {
namespace clock_tree_synthesis {
class clock_tree_synthesis
{
    entity_system::entity_system m_flip_flops_system;

    flip_flops m_flip_flops;
public:
    clock_tree_synthesis();
    ~clock_tree_synthesis();

    entity_system::entity flip_flop_insert(entity_system::entity netlist_cell);

    void flip_flop_remove(entity_system::entity flip_flop);

    std::size_t flip_flop_count() const {
        return m_flip_flops_system.size();
    }

    entity_system::entity flip_flop_cell(entity_system::entity flip_flop) const {
        return m_flip_flops.netlist_cell(flip_flop);
    }

    const entity_system::entity_system & flip_flop_system() const {
        return m_flip_flops_system;
    }
};
}
}

#endif // CLOCK_TREE_SYNTHESIS_H
