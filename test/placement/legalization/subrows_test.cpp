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

#include "../../catch.hpp"
#include "subrows.h"

TEST_CASE("legalization/create subrows","[legalization][subrows]") {
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::netlist::netlist netlist(&std_cells);
    ophidian::placement::library lib(&std_cells);
    ophidian::placement::placement placement(&netlist, &lib);
    ophidian::floorplan::floorplan floorplan;

    auto std_cell = std_cells.cell_create("inv");
    ophidian::geometry::multi_polygon<ophidian::geometry::polygon<ophidian::geometry::point<double> > > cell_geometry;
    std::vector<ophidian::geometry::point<double> > points { {0.0, 0.0}, {0.0, 10.0}, {10.0, 10.0}, {10.0, 0.0}, {0.0, 0.0} };
    ophidian::geometry::polygon<ophidian::geometry::point<double> > polygon;
    boost::geometry::append(polygon, points);
    cell_geometry.push_back(polygon);
    lib.geometry(std_cell, cell_geometry);
    auto cell = netlist.cell_insert("cell", "inv");
    auto site = floorplan.site_insert("core", {10, 10});
    floorplan.row_insert(site, 100, {0, 0});
    placement.cell_position(cell, {100, 0});
    placement.cell_fixed(cell, true);

    ophidian::entity::system subrows_system;
    ophidian::placement::legalization::subrows subrows(subrows_system);
    subrows.create_subrows(&floorplan, &placement);
    REQUIRE(subrows.count() == 2);

    auto first_subrow = subrows.find_subrow({0, 0});
    auto second_subrow = subrows.find_subrow({150, 0});
    REQUIRE(subrows.begin(first_subrow) == 0.0);
    REQUIRE(subrows.end(first_subrow) == 99.0);
    REQUIRE(subrows.begin(second_subrow) == 110.0);
    REQUIRE(subrows.end(second_subrow) == 999.0);
}