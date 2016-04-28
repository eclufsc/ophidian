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
#include <iostream>
#include <fstream>
#include <lef.h>

#include "abu.h"
#include "def.h"

TEST_CASE("density/ abu of superblue18","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue18/superblue18.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.85;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0401271;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue16","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue16/superblue16.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.85;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0333554;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue4","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue4/superblue4.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.90;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0439752;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue10","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue10/superblue10.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.87;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0417409;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue7","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue7/superblue7.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.90;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0296557;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue1","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue1/superblue1.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.80;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0536546;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue3","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue3/superblue3.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.87;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0287357;

    REQUIRE(measured_abu == Approx(golden_abu));
}

TEST_CASE("density/ abu of superblue5","[density][abu][regression]") {
    std::ifstream def("benchmarks/superblue5/superblue5.def", std::ifstream::in);
    REQUIRE( def.good() );
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement cells(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;
    // TODO READ LEF
    ophidian::placement::def::read(def, &netlist, &cells, &floorplan);

    auto row_it = floorplan.rows_system().begin();
    double row_height = floorplan.row_dimensions(row_it->first).y();
    unsigned number_of_rows_in_each_bin = 9;

    ophidian::density::abu abu(&floorplan, &cells, {number_of_rows_in_each_bin * row_height, number_of_rows_in_each_bin * row_height});
    double target_utilization = 0.85;
    double measured_abu = abu.measure_abu(target_utilization);
    double golden_abu = 0.0208419;

    REQUIRE(measured_abu == Approx(golden_abu));
}
