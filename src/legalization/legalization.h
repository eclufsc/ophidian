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

#ifndef OPHIDIAN_LEGALIZATION_H
#define OPHIDIAN_LEGALIZATION_H

#include "floorplan.h"
#include "placement.h"
#include "subrows.h"
#include <entity_system.h>

namespace ophidian {
namespace legalization {
class legalization {
protected:
    entity_system::entity_system m_subrows_system;
    subrows m_subrows;

    floorplan::floorplan * m_floorplan;
    placement::placement * m_placement;
public:

    legalization(floorplan::floorplan * floorplan, placement::placement *placement)
        : m_subrows(m_subrows_system), m_floorplan(floorplan), m_placement(placement) {
        create_subrows();
    }
    virtual ~legalization() {}

    virtual void legalize_placement() = 0;

    void create_subrows();
};
}
}



#endif //OPHIDIAN_LEGALIZATION_H
