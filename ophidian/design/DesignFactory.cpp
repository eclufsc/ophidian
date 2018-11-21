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

#include "Design.h"

#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/circuit/NetlistFactory.h>
#include <ophidian/placement/LibraryFactory.h>
#include <ophidian/placement/PlacementFactory.h>
#include <ophidian/floorplan/FloorplanFactory.h>
#include <ophidian/routing/LibraryFactory.h>
#include <ophidian/routing/GlobalRoutingFactory.h>

namespace ophidian::design::factory
{
    void make_design(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept
    {
        floorplan::factory::make_floorplan(design.floorplan(), def, lef);

        circuit::factory::make_standard_cells(design.standard_cells(), lef);

        circuit::factory::make_netlist(design.netlist(), verilog, design.standard_cells());

        placement::factory::make_library(design.placement_library(), lef, design.standard_cells());

        placement::factory::make_placement(design.placement(), def, design.netlist());
    }

    void make_design_iccad2015(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept
    {
        return make_design(design, def, lef, verilog);
    }

    void make_design_iccad2017(Design& design, const parser::Def& def, const parser::Lef& lef) noexcept
    {
        floorplan::factory::make_floorplan(design.floorplan(), def, lef);

        circuit::factory::make_standard_cells(design.standard_cells(), lef);

        circuit::factory::make_netlist(design.netlist(), def, design.standard_cells());

        placement::factory::make_library(design.placement_library(), lef, design.standard_cells());

        placement::factory::make_placement(design.placement(), def, design.netlist());
    }

    void make_design_ispd2018(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Guide &guide) noexcept
    {
        floorplan::factory::make_floorplan(design.floorplan(), def, lef);

        circuit::factory::make_standard_cells(design.standard_cells(), lef);

        circuit::factory::make_netlist(design.netlist(), def, design.standard_cells());

        placement::factory::make_library(design.placement_library(), lef, design.standard_cells());

        placement::factory::make_placement(design.placement(), def, design.netlist());

        routing::factory::make_library(design.routing_library(), lef, def);

        routing::factory::make_global_routing(design.global_routing(), design.routing_library(), design.netlist(), guide);
    }
}
