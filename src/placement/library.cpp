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

#include "library.h"

namespace ophidian {
namespace placement {

library::library(ophidian::standard_cell::standard_cells * std_cells) :
    m_std_cells(*std_cells) {
    std_cells->register_cell_property(&m_cell_geometry);
    std_cells->register_pin_property(&m_pin_offset);
}

library::~library() {
}

void library::geometry(entity::entity cell, multipolygon geometry) {
    m_cell_geometry[m_std_cells.cell_system().lookup(cell)] = geometry;
}

entity::entity library::cell_create(std::string name)
{
    return m_std_cells.cell_create(name);
}

void library::dist2microns(int32_t dist) {
    m_dist2microns = dist;
}
void library::pin_offset(entity::entity pin, point offset) {
    m_pin_offset[m_std_cells.pin_system().lookup(pin)] = offset;
}

entity::entity library::pin_create(entity::entity cell, std::string name)
{
    return m_std_cells.pin_create(cell, name);
}


} /* namespace placement */
} /* namespace ophidian */

