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

#include "../timing/library_timing_arcs.h"

TEST_CASE("library timing arc/empty", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	REQUIRE(tarcs.size() == 0);
}

TEST_CASE("library timing arc/create, check from & to", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.size() == 1);
	REQUIRE(tarcs.from(arc) == INV_X1a);
	REQUIRE(tarcs.to(arc) == INV_X1o);
}

TEST_CASE("library timing arc/assign pins to timing arc", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	auto arcs = tarcs.pin_timing_arcs(INV_X1a);
	REQUIRE(arcs.size() == 1);
	REQUIRE(arcs.front() == arc);
	auto arcs2 = tarcs.pin_timing_arcs(INV_X1o);
	REQUIRE(arcs2 == arcs);
}

TEST_CASE("library timing arc/get", "[timing][librarytimingarc]") {
	ophidian::standard_cell::standard_cells std_cells;
	auto INV_X1 = std_cells.cell_create("INV_X1");
	auto INV_X1a = std_cells.pin_create(INV_X1, "a");
	auto INV_X1o = std_cells.pin_create(INV_X1, "o");
	ophidian::timing::library_timing_arcs tarcs(&std_cells);
	auto arc = tarcs.create(INV_X1a, INV_X1o);
	REQUIRE(tarcs.get(INV_X1a, INV_X1o) == arc);
}
