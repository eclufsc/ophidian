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
/*
#include "../catch.hpp"

#include "../parsing/lef.h"
#include "../parsing/def.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"
#include "../floorplan/lefdef2floorplan.h"

#include "abu.h"

using namespace ophidian;

TEST_CASE("density/ abu of simple","[density][abu]") {
    double target_utilization = 0.7;
    double golden_abu = 0.0125911;

    standard_cell::standard_cells std_cells;
    netlist::netlist netlist(&std_cells);
    placement::library lib(&std_cells);
    placement::placement cells(&netlist, &lib);
    floorplan::floorplan floorplan;

    const std::string dot_lef_file{"input_files/vga_lcd.lef"};
    const std::string dot_def_file{"input_files/vga_lcd.def"};

    std::unique_ptr<parsing::lef> lef;
    std::unique_ptr<parsing::def> def;

#pragma omp single nowait
    {
#pragma omp task shared(lef, dot_lef_file)
        lef.reset(new parsing::lef(dot_lef_file));
#pragma omp task shared(def, dot_def_file)
        def.reset(new parsing::def(dot_def_file));
    }
#pragma omp taskwait

    placement::lef2library(*lef, lib);
    placement::def2placement(*def, cells);
    floorplan::lefdef2floorplan(*lef, *def, floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(*row_it).y();
    unsigned number_of_rows_in_each_bin = 9;

    density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double measured_abu = abu.measure_abu(target_utilization);
    REQUIRE(Approx(measured_abu) == golden_abu);
}*/
