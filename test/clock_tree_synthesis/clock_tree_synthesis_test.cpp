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

#include "clock_tree_synthesis.h"
#include "extract_flip_flops.h"
#include "verilog.h"
#include "verilog2netlist.h"
#include "liberty.h"
#include "../catch.hpp"

TEST_CASE("clock_tree_synthesis/ empty","[clock_tree_synthesis]") {
    ophidian::clock_tree_synthesis::clock_tree_synthesis clock_tree_synthesis;

    REQUIRE(clock_tree_synthesis.flip_flop_count() == 0);
}

TEST_CASE("clock_tree_synthesis/ insert flip flop","[clock_tree_synthesis]") {
    ophidian::clock_tree_synthesis::clock_tree_synthesis clock_tree_synthesis;

    ophidian::entity_system::entity_system cell_system;
    auto cell1 = cell_system.create();

    auto flip_flop1 = clock_tree_synthesis.flip_flop_insert(cell1);

    REQUIRE(clock_tree_synthesis.flip_flop_count() == 1);
    REQUIRE(clock_tree_synthesis.flip_flop_cell(flip_flop1) == cell1);
}

TEST_CASE("clock_tree_synthesis/ remove flip flop","[clock_tree_synthesis]") {
    ophidian::clock_tree_synthesis::clock_tree_synthesis clock_tree_synthesis;

    ophidian::entity_system::entity_system cell_system;
    auto cell1 = cell_system.create();

    auto flip_flop1 = clock_tree_synthesis.flip_flop_insert(cell1);

    clock_tree_synthesis.flip_flop_remove(flip_flop1);

    REQUIRE(clock_tree_synthesis.flip_flop_count() == 0);
    REQUIRE_THROWS(clock_tree_synthesis.flip_flop_cell(flip_flop1));
}

TEST_CASE("clock_tree_synthesis/ create clock tree synthesis with simple flip flops","[clock_tree_synthesis]") {
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);

    ophidian::parsing::verilog verilog("input_files/simple.v");
    ophidian::netlist::verilog2netlist(verilog, netlist);

    ophidian::timing::library_timing_arcs tarcs(&std_cells);
    ophidian::timing::library library(&tarcs, &std_cells);
    ophidian::timing::liberty::read("input_files/simple_Late.lib", library);

    ophidian::clock_tree_synthesis::clock_tree_synthesis clock_tree_synthesis;
    ophidian::clock_tree_synthesis::extract_flip_flops(&netlist, &std_cells, &clock_tree_synthesis);

    REQUIRE(clock_tree_synthesis.flip_flop_count() == 1);
    auto flip_flop = clock_tree_synthesis.flip_flop_system().entities().front();
    REQUIRE(netlist.cell_name(clock_tree_synthesis.flip_flop_cell(flip_flop)) == "f1");
}
