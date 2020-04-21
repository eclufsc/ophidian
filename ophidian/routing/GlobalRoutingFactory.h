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

#ifndef OPHIDIAN_GLOBAL_ROUTING_FACTORY_H
#define OPHIDIAN_GLOBAL_ROUTING_FACTORY_H

#include <ophidian/parser/Guide.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/ICCAD2020.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/routing/Library.h>
#include <ophidian/routing/RoutingConstraints.h>
#include "GlobalRouting.h"

namespace ophidian::routing::factory
{
    void make_global_routing(GlobalRouting& globalRouting,
                             const Library & library,
                             const ophidian::circuit::Netlist & netlist,
                             const ophidian::parser::Guide& guide,
                             const ophidian::parser::Def& def
                            ) noexcept;

    void make_global_routing(GlobalRouting& globalRouting,
                             const Library & library,
                             const RoutingConstraints & routing_constraints,
                             const ophidian::circuit::Netlist & netlist,
                             const ophidian::circuit::StandardCells & std_cells,
                             const ophidian::parser::ICCAD2020 & iccad_2020) noexcept;
}

#endif // OPHIDIAN_GLOBAL_ROUTING_FACTORY_H
