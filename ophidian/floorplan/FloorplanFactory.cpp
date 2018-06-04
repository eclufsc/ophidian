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

namespace ophidian
{
namespace floorplan
{
    namespace factory
    {
        Floorplan make_floorplan(const parser::Def & def, const parser::Lef & lef)
        {
            auto floorplan = Floorplan{};

            floorplan.chipOrigin(def.die_area().min_corner());
            floorplan.chipUpperRightCorner(def.die_area().max_corner());

            for(const auto& site : lef.sites())
            {
                floorplan.add(
                    Site{},
                    site.name(),
                    util::LocationDbu{site.width() * lef.micrometer_to_dbu_ratio(), site.height() * lef.micrometer_to_dbu_ratio()}
                );
            }

            for(const auto & row : def.rows())
            {
                floorplan.add(
                    Row{},
                    row.origin(),
                    row.num().x(),
                    floorplan.find(row.site()));
            }

            return floorplan;
        }
    }
}     // namespace floorplan
}     // namespace ophidian
