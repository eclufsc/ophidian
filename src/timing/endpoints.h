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

#ifndef OPHIDIAN_TIMING_ENDPOINTS_H
#define OPHIDIAN_TIMING_ENDPOINTS_H

#include "../entity_system/entity_system.h"
#include "../netlist/netlist.h"

namespace ophidian {
namespace timing {

class endpoints
{
    std::vector< entity_system::entity > m_entities;

public:
    endpoints();
    endpoints(const netlist::netlist & netlist);
    virtual ~endpoints();

    std::vector< entity_system::entity >::const_iterator begin() const {
        return m_entities.begin();
    }
    std::vector< entity_system::entity >::const_iterator end() const {
        return m_entities.end();
    }
    std::size_t size() const {
        return m_entities.size();
    }
    bool empty() const {
        return m_entities.empty();
    }

};

}
}

#endif // OPHIDIAN_TIMING_ENDPOINTS_H
