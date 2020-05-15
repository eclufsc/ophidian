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

#include "FloorplanFactory.h"

namespace ophidian::floorplan::factory
{
    void make_floorplan(Floorplan& floorplan, const parser::Def & def, const parser::Lef & lef)
    {
        floorplan.chip_origin() = def.die_area().min_corner();
        floorplan.chip_upper_right_corner() = def.die_area().max_corner();

        for(const auto& site : lef.sites())
        {
            floorplan.add_site(
                site.name(),
                Floorplan::point_type{site.width() * lef.micrometer_to_dbu_ratio(), site.height() * lef.micrometer_to_dbu_ratio()}
            );
        }

        for(const auto & row : def.rows())
        {
            floorplan.add_row(
                row.origin(),
                row.num().x(),
                floorplan.find(row.site())
            );
        }
    }

    void make_floorplan(Floorplan& floorplan, const parser::ICCAD2020 & iccad_2020)
    {
        using unit_type = ophidian::floorplan::Floorplan::unit_type;
        using point_type = ophidian::floorplan::Floorplan::point_type;

        auto dimensions = iccad_2020.grid_dimensions();
        auto gcell_size = 10.0;
        auto upper_x = std::get<0>(dimensions) * gcell_size;
        auto upper_y = std::get<1>(dimensions) * gcell_size;

        floorplan.chip_origin() = point_type(unit_type(0.0), unit_type(0.0));
        floorplan.chip_upper_right_corner() = point_type(unit_type(upper_x), unit_type(upper_y));
    }
}
