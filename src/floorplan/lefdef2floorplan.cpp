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

#include "lefdef2floorplan.h"

namespace ophidian {
namespace floorplan {

void lefdef2floorplan(const parsing::lef &lef, const parsing::def &def, floorplan &fplan)
{
    fplan.chip_origin({def.die().lower.x, def.die().lower.y});
    fplan.chip_boundaries({def.die().upper.x, def.die().upper.y});
    for(const parsing::lef::site & site : lef.sites())
        fplan.site_insert(site.name, {site.x * lef.database_units(), site.y * lef.database_units()});
    for(const parsing::def::row & row : def.rows())
        fplan.row_insert(row.site, row.num.x, {row.origin.x, row.origin.y});
}

}
}

