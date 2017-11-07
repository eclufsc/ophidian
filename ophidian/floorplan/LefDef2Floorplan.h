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

#ifndef OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H
#define OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H

#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>

#include "Floorplan.h"

namespace ophidian
{
namespace floorplan
{
void lefDef2Floorplan(const parser::Lef &lef, const parser::Def &def, Floorplan &floorplan);
} // namespace floorplan
} // namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_LEFDEF2FLOORPLAN_H
