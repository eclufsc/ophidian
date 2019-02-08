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
        using fence_region_name_type = std::string;
        using cell_instance_type = circuit::CellInstance;
        using fence_region_geometry_type = geometry::CellGeometry;
        using members_view_type = entity_system::Aggregation<fence_region_type, cell_instance_type>::Parts;
        using fence_region_container_type = entity_system::EntitySystem<fence_region_type>;

        FenceRegions() = delete;

        FenceRegions(const FenceRegions&) = default;
        FenceRegions& operator=(FenceRegions&) = default;

        FenceRegions(FenceRegions&&) = default;
        FenceRegions& operator=(FenceRegions&&) = default;

        FenceRegions(circuit::Netlist & netlist);

        fence_region_type add_fence_region(const fence_region_name_type & name) noexcept;

        void add_member(const fence_region_type& fence_region, const cell_instance_type & cell) noexcept;

        void add_box(const fence_region_type& fence_region, const fence_region_geometry_type::box_type & box) noexcept;

        const fence_region_name_type & name(const fence_region_type & fence_region) const noexcept;

        const fence_region_geometry_type & area(const fence_region_type & fence_region) const noexcept;

        members_view_type members(const fence_region_type & fence_region) const noexcept;

        fence_region_type fence_region(const cell_instance_type & cell) const noexcept;

        bool has_fence(const cell_instance_type & cell) const noexcept;

        fence_region_container_type::size_type size_fence_region() const noexcept;

        fence_region_type find_fence_region(const fence_region_name_type & name) const noexcept;

    private:
        entity_system::EntitySystem<fence_region_type> m_fence_regions;

        entity_system::Property<fence_region_type, fence_region_name_type> m_fence_names;
        entity_system::Property<fence_region_type, fence_region_geometry_type> m_fence_areas;

        entity_system::Aggregation<fence_region_type, cell_instance_type> m_fence_members;

        std::unordered_map<fence_region_name_type, fence_region_type> m_name_to_fence_region;
    };

}

#endif