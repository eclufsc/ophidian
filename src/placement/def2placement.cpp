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

#include "def2placement.h"

namespace ophidian {
namespace placement {

void def2placement(const parsing::def &def, placement &place)
{
    for(auto & component : def.components())
    {
        auto the_cell = place.cell_create(component.name, component.macro);
        place.cell_position(the_cell, {component.position.x, component.position.y});
        place.cell_fixed(the_cell, component.fixed);
    }
}

}
}
