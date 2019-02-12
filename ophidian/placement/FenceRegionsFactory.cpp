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

#include "FenceRegionsFactory.h"

namespace ophidian::placement::factory {

    void make_fence_regions(FenceRegions & fence_regions, const parser::Def & def, const floorplan::Floorplan & floorplan, circuit::Netlist & netlist)
    {
        auto groups = def.groups();

        for (auto region : def.regions()) {
            auto fence_region = fence_regions.add_fence_region(region.name());

            auto chip_box = FenceRegions::fence_region_geometry_type::box_type{floorplan.chip_origin(), floorplan.chip_upper_right_corner()};

            for (auto box : region.rectangles()) {
                auto intersection = FenceRegions::fence_region_geometry_type::box_type{};
                boost::geometry::intersection(box, chip_box, intersection);
                fence_regions.add_box(fence_region, intersection);
            }

            auto region_group = groups[region.name()];
            auto region_members = region_group.members();

            std::string members_regex_str =
            std::accumulate(region_members.begin(), region_members.end(), std::string(),
                            [](const std::string& a, const std::string& b) -> std::string {
                    return a + (a.length() > 0 ? "|" : "") + b;
                });
            boost::replace_all(members_regex_str, "*", ".*");
            boost::replace_all(members_regex_str, "/", "\\/");

            auto members_regex = std::regex(members_regex_str);

            for (auto component : def.components()) {
                if (std::regex_match(component.name(), members_regex)) {
                    auto cell = netlist.add_cell_instance(component.name());

                    fence_regions.add_member(fence_region, cell);
                }
            }
        }
    }

}