/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_PLACEMENT_FENCE_REGIONS_H
#define OPHIDIAN_PLACEMENT_FENCE_REGIONS_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/geometry/CellGeometry.h>

namespace ophidian::placement {
    class FenceRegion : public entity_system::EntityBase 
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class FenceRegions 
    {
    public:
        using fence_region_type = FenceRegion;
        using cell_instance_type = circuit::CellInstance;
        using cell_geometry_type = geometry::CellGeometry;

        FenceRegions() = delete;

        FenceRegions(const FenceRegions&) = default;
        FenceRegions& operator=(FenceRegions&) = default;

        FenceRegions(FenceRegions&&) = default;
        FenceRegions& operator=(FenceRegions&&) = default;

        FenceRegions(circuit::Netlist & netlist);

    private:
        entity_system::EntitySystem<fence_region_type> m_fence_regions;

        entity_system::Property<fence_region_type, std::string> m_fence_names;
        entity_system::Property<fence_region_type, cell_geometry_type> m_fence_areas;

        entity_system::Aggregation<fence_region_type, cell_instance_type> m_fence_cells;
    };

}

#endif