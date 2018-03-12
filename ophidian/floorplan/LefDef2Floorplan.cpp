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

#include "LefDef2Floorplan.h"

namespace ophidian
{
    namespace floorplan
    {
        void lefDef2Floorplan(
            const parser::Lef & lef,
            const parser::Def & def,
            Floorplan & floorplan)
        {
            floorplan.chipOrigin(util::LocationDbu(def.die().lower.x, def.die().lower.y));
            floorplan.chipUpperRightCorner(util::LocationDbu(def.die().upper.x, def.die().upper.y));
            for(auto & site : lef.sites())
            {
                floorplan.add(
                    Site(),
                    site.name,
                    util::LocationDbu(site.x * lef.databaseUnits(), site.y * lef.databaseUnits()));
            }
            for(auto & row : def.rows())
            {
                floorplan.add(
                    Row(),
                    util::LocationDbu(row.origin.x, row.origin.y),
                    row.num.x,
                    floorplan.find(row.site));
            }
        }
    }     // namespace floorplan
}     // namespace ophidian