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

#ifndef OPHIDIAN_SRC_METAL_LAYERS_H
#define OPHIDIAN_SRC_METAL_LAYERS_H

#include "../entity_system/vector_property.h"
#include "../entity_system/entity_system.h"

namespace ophidian{
/// Namespace describing routing entities and basic routing interface.
namespace routing{

enum class ROUTING_DIRECTION {HORIZONTAL, VERTICAL};

class metal_layers
{
    ophidian::entity_system::entity_system m_metal_layers_system;

    entity_system::vector_property<ROUTING_DIRECTION> m_routing_direction;
    entity_system::vector_property<double> m_pitch;
    entity_system::vector_property<double> m_width;

//    entity::vector_property<ROUTING_DIRECTION> m_resistance_per_micron;
//    entity::vector_property<ROUTING_DIRECTION> m_capacitance_per_micron;

//    entity::vector_property<ROUTING_DIRECTION> m_via_resistance_with_upper_layer;
//    entity::vector_property<ROUTING_DIRECTION> m_via_capacitance_with_upper_layer;

public:
    metal_layers();
    virtual ~metal_layers();

    entity_system::entity layer_insert(ROUTING_DIRECTION direction, double pitch, double width);
};

}  /* namespace routing */
}  /* namespace ophidian */

#endif // OPHIDIAN_SRC_METAL_LAYERS_H
