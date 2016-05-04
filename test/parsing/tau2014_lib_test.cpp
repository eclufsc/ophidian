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
#include "../parsing/tau2014/linear_library.h"

using namespace ophidian;

struct tau2014_moc {
    parsing::tau2014::linear_library lib{"input_files/cell.lib"};
};

//TEST_CASE_METHOD()

TEST_CASE_METHOD(tau2014_moc, "tau2014 lib/read", "[parsing][tau2014]")
{

    REQUIRE( lib.cells().at("AND2_X1").input_pins[0].name == "A1" );
    REQUIRE( lib.cells().at("AND2_X1").input_pins[1].name == "A2" );
    REQUIRE( Approx(lib.cells().at("AND2_X1").input_pins[0].rise_capacitance) == 8.94119e-16 );
    REQUIRE( Approx(lib.cells().at("AND2_X1").input_pins[1].fall_capacitance) == 9.7463e-16 );

    const parsing::tau2014::linear_library::cell & and3_x1 = lib.cells().at("AND3_X1");
    REQUIRE(and3_x1.arcs.at("A1").size() == 1);
    const parsing::tau2014::linear_library::arc & arc = and3_x1.arcs.at("A1").at("ZN");
    // slew fall 4.07129e-12 965.459 0.0225452
    //      rise 4.2905e-12 2242.54 0.00826234
    // delay fall 4.02891e-11 1254.64 0.278738
    //       rise 4.78751e-11 2404.71 0.160859
    REQUIRE( Approx(arc.fall_slew[0]) == 4.07129e-12 );
    REQUIRE( Approx(arc.fall_slew[1]) == 965.459 );
    REQUIRE( Approx(arc.fall_slew[2]) == 0.0225452 );

    REQUIRE( Approx(arc.rise_slew[0]) == 4.2905e-12 );
    REQUIRE( Approx(arc.rise_slew[1]) == 2242.54 );
    REQUIRE( Approx(arc.rise_slew[2]) == 0.00826234 );

    REQUIRE( Approx(arc.fall_delay[0]) == 4.02891e-11 );
    REQUIRE( Approx(arc.fall_delay[1]) == 1254.64 );
    REQUIRE( Approx(arc.fall_delay[2]) == 0.278738 );

    REQUIRE( Approx(arc.rise_delay[0]) == 4.78751e-11 );
    REQUIRE( Approx(arc.rise_delay[1]) == 2404.71 );
    REQUIRE( Approx(arc.rise_delay[2]) == 0.160859 );


    const parsing::tau2014::linear_library::cell & flop = lib.cells().at("DFF_X2");

    REQUIRE( flop.name == "DFF_X2" );
    REQUIRE( flop.input_pins.size() == 2 );
    REQUIRE( flop.input_pins[flop.clock_pin_id].name == "CK" );
    REQUIRE( flop.input_pins[flop.data_pin_id].name == "D" );
    REQUIRE( flop.output_pins.size() == 2 );
    REQUIRE( !flop.setup.empty() );
    REQUIRE( !flop.hold.empty() );

}


#include "../standard_cell/standard_cells.h"
#include "../timing/library_timing_arcs.h"
#include "../timing/library.h"
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>

using namespace boost::units;
using namespace boost::units::si;

#include "../timing/tau2015lib2library.h"

TEST_CASE_METHOD(tau2014_moc, "tau2014 lib/to library", "[parsing][tau2014]")
{
    standard_cell::standard_cells std_cells;
    timing::library_timing_arcs tarcs{&std_cells};
    timing::library library{&tarcs, &std_cells};
    timing::tau2015lib2library(lib, library);

//    cell XOR2_X1
//    pin A input 2.18123e-15 2.23214e-15
//    pin B input 2.41145e-15 2.36355e-15
//    pin Z output
//    timing A Z non_unate 2.74617e-12 1389.18 0.230104 0.0 0.0 0.0 0.0 0.0 0.0 6.11835e-12 4051.12 0.174039 0.0 0.0 0.0 0.0 0.0 0.0 1.05298e-11 2978.3 0.160317 0.0 0.0 0.0 0.0 0.0 0.0 1.11141e-11 5127.34 0.484998 0.0 0.0 0.0 0.0 0.0 0.0
//    timing B Z non_unate 2.27421e-12 1491.28 0.1428 0.0 0.0 0.0 0.0 0.0 0.0 9.03653e-12 4136.79 0.156073 0.0 0.0 0.0 0.0 0.0 0.0 1.05089e-11 2671.36 0.160041 0.0 0.0 0.0 0.0 0.0 0.0 1.59324e-11 5057.17 0.500546 0.0 0.0 0.0 0.0 0.0 0.0


    const parsing::tau2014::linear_library::cell & XOR2_X1 = lib.cells().at("XOR2_X1");
    auto pin_timing_arcs = library.pin_timing_arcs(library.pin_create(library.cell_create(XOR2_X1.name), XOR2_X1.input_pins.front().name));


    REQUIRE(pin_timing_arcs.size() == 1);

    for(auto tarc : pin_timing_arcs)
    {
        auto from_entity = library.timing_arc_from(tarc);
        auto to_entity = library.timing_arc_to(tarc);
        REQUIRE( std_cells.pin_direction(from_entity) == standard_cell::pin_directions::INPUT );
        REQUIRE( std_cells.pin_direction(to_entity) == standard_cell::pin_directions::OUTPUT );
        REQUIRE( Approx(library.timing_arc_fall_slew(tarc).compute(quantity<capacitance>(5*femto*farads), quantity<si::time>(80*pico*seconds)).value()) == 2.74617e-12+1389.18*5e-15*0.230104*80e-12);
    }

}
