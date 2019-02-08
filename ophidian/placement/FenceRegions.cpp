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

#include "FenceRegions.h"

namespace ophidian::placement {

    FenceRegions::FenceRegions(circuit::Netlist & netlist)
    : m_fence_names(m_fence_regions), m_fence_areas(m_fence_regions),
      m_fence_members(netlist.make_aggregation_to_cell_instance(m_fence_regions))
    {

    }

    FenceRegions::fence_region_type FenceRegions::add_fence_region(const fence_region_name_type & name) noexcept
    {
        auto fence_region = m_fence_regions.add();
        m_fence_names[fence_region] = name;
        return fence_region;
    }

    void FenceRegions::add_member(const fence_region_type& fence_region, const cell_instance_type & cell) noexcept
    {
        m_fence_members.addAssociation(fence_region, cell);
    }

    void FenceRegions::add_box(const fence_region_type& fence_region, const fence_region_geometry_type::box_type & box) noexcept
    {
        m_fence_areas[fence_region].push_back(box);
    }

    const FenceRegions::fence_region_name_type & FenceRegions::name(const fence_region_type & fence_region) const noexcept
    {
        return m_fence_names[fence_region];
    }

    const FenceRegions::fence_region_geometry_type & FenceRegions::area(const fence_region_type & fence_region) const noexcept
    {
        return m_fence_areas[fence_region];
    }

    FenceRegions::members_view_type FenceRegions::members(const fence_region_type & fence_region) const noexcept
    {
        return m_fence_members.parts(fence_region);
    }

    FenceRegions::fence_region_type FenceRegions::fence_region(const cell_instance_type & cell) const noexcept
    {
        return m_fence_members.whole(cell);
    }

    bool FenceRegions::has_fence(const cell_instance_type & cell) const noexcept
    {
        auto fence = fence_region(cell);
        return fence != FenceRegion();
    }

}