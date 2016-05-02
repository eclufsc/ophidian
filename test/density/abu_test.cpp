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

#include "../catch.hpp"

#include "../parsing/lef.h"
#include "../parsing/def.h"

#include "../placement/def2placement.h"
#include "../placement/lef2library.h"

#include "abu.h"

using namespace ophidian;

bool run_circuit(const std::string & ckt_name, const double target_utilization, const double golden_abu) {
    standard_cell::standard_cells std_cells;
    netlist::netlist netlist(&std_cells);
    placement::library lib(&std_cells);
    placement::placement cells(&netlist, &lib);
    floorplan::floorplan floorplan;

    const std::string dot_lef_file{"benchmarks/" + ckt_name + "/" + ckt_name + ".lef"};
    const std::string dot_def_file{"benchmarks/" + ckt_name + "/" + ckt_name + ".def"};

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

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double measured_abu = abu.measure_abu(target_utilization);
    return Approx(measured_abu) == golden_abu;
}



TEST_CASE("density/ abu of iccad2015 ckts","[density][abu][regression][iccad2015]") {
    REQUIRE(run_circuit("superblue18", 0.85, 0.0401271));
    REQUIRE(run_circuit("superblue16", 0.85, 0.0333554));
    REQUIRE(run_circuit("superblue4", 0.9, 0.0439752));
    REQUIRE(run_circuit("superblue10", 0.87, 0.0417409));
    REQUIRE(run_circuit("superblue7", 0.9, 0.0296557));
    REQUIRE(run_circuit("superblue1", 0.8, 0.0536546));
    REQUIRE(run_circuit("superblue3", 0.87, 0.0287357));
    REQUIRE(run_circuit("superblue5", 0.85, 0.0208419));
}
