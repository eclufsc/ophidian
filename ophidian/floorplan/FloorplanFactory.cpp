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
}
