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

namespace ophidian::design::factory
{
    Design make_design(const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept
    {
        auto floorplan = floorplan::factory::make_floorplan(def, lef);

        auto standard_cells = circuit::factory::make_standard_cells(lef);

        auto netlist = circuit::factory::make_netlist(verilog, standard_cells);

        auto library = placement::factory::make_library(lef, standard_cells);

        auto placement = placement::factory::make_placement(def, netlist, library);

        return Design{std::move(floorplan), std::move(standard_cells), std::move(netlist), std::move(library), std::move(placement)};
    }

    Design make_design_iccad2015(const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept
    {
        return make_design(def, lef, verilog);
    }

    Design make_design_iccad2017(const parser::Def& def, const parser::Lef& lef) noexcept
    {
        auto design = Design{};

        auto floorplan = floorplan::factory::make_floorplan(def, lef);

        auto standard_cells = circuit::factory::make_standard_cells(lef);

        auto netlist = circuit::Netlist{};

        auto library = placement::factory::make_library(lef, standard_cells);

        auto placement = placement::factory::make_placement(def, netlist, library);

        return Design{std::move(floorplan), std::move(standard_cells), std::move(netlist), std::move(library), std::move(placement)};
    }
}
