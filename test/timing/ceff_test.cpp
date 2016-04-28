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
#include "../interconnection/rc_tree.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>

#include <lemon/list_graph.h>
#include <lemon/bfs.h>
#include <lemon/connectivity.h>


#include "../timing/liberty.h"
#include <boost/units/cmath.hpp>


#include "../timing/ceff.h"
#include "../timing/elmore.h"
#include "../timing/elmore_second_moment.h"

using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;

struct simple_driver_model {
    timing::library & lib;
    entity::entity tarc;
    const quantity<si::time> slew;
    quantity<si::time> operator()(const quantity<capacitance> load) const {
        return lib.timing_arc_rise_slew(tarc).compute(load, slew);
    }
};

TEST_CASE("simple test", "[timing][ceff]")
{

    interconnection::rc_tree tree;
    const quantity<si::time> input_slew(80*pico*seconds);

    /*
             R4        R5=0ohms (virtual resistance
          __/\/\/\___/\/\/\___
         |         |        |
         |         = u2:a   = u2:a (pin_capacitance)
         |         | (int.) |
         |         z        z
    R1   |   R2       R3 = 0ohms (virtual resistance)
 _/\/\/\_|__/\/\/\___/\/\/\__
|        |        |        |
= c0     = c1     = u1:a   = u1:a
|        |        | (int.) | (pin capacitance)
z        z        z        z


    */

    auto c0 = tree.capacitor_insert("c0 (source node)");
    auto c1 = tree.capacitor_insert("c1 (internal node)");
    auto u1_a_internal = tree.capacitor_insert("u1:a (internal node)");
    auto u1_a = tree.capacitor_insert("u1:a (tap node)");
    auto u2_a_internal = tree.capacitor_insert("u2:a (internal node)");
    auto u2_a = tree.capacitor_insert("u2:a (tap node)");

    tree.capacitance(c0, quantity<capacitance>(1*femto*farad));
    tree.capacitance(c1, quantity<capacitance>(4*femto*farad));
    tree.capacitance(u1_a_internal, quantity<capacitance>(1*femto*farad));
    tree.capacitance(u1_a, quantity<capacitance>(2*femto*farad));
    tree.capacitance(u2_a_internal, quantity<capacitance>(2*femto*farad));
    tree.capacitance(u2_a, quantity<capacitance>(2*femto*farad));

    tree.resistor_insert(c0, c1, quantity<resistance>(0.2*kilo*ohms));
    tree.resistor_insert(c1, u1_a_internal, quantity<resistance>(0.2*kilo*ohms));
    tree.resistor_insert(c1, u2_a_internal, quantity<resistance>(0.4*kilo*ohms));
    tree.resistor_insert(u1_a_internal, u1_a, quantity<resistance>(0.0*ohms));
    tree.resistor_insert(u2_a_internal, u2_a, quantity<resistance>(0.0*ohms));

    standard_cell::standard_cells std_cell;
    timing::library_timing_arcs tarcs{&std_cell};
    timing::library lib{&tarcs, &std_cell};
    timing::liberty::read("benchmarks/superblue16/superblue16_Late.lib", lib);


    auto tarc = lib.timing_arc(std_cell.pin_create(std_cell.cell_create("INV_X1"), "a"), std_cell.pin_create(std_cell.cell_create("INV_X1"), "o"));

    simple_driver_model s_calculator{lib, tarc, input_slew};
    timing::effective_capacitance_wire_model calculator;
    auto ceff = calculator.simulate(s_calculator, tree, c0);

    REQUIRE( ceff < tree.lumped() );
    std::cout << "rc network lumped capacitance " << tree.lumped() << " effective capacitance " << ceff << std::endl;
    std::cout << "delay to tap node " << tree.capacitor_name(u1_a) << " = " << calculator.delays()[u1_a] << std::endl;
    std::cout << "delay to tap node " << tree.capacitor_name(u2_a) << " = " << calculator.delays()[u2_a] << std::endl;
    std::cout << "slew to tap node " << tree.capacitor_name(u1_a) << " = " << calculator.slews()[u1_a] << std::endl;
    std::cout << "slew to tap node " << tree.capacitor_name(u2_a) << " = " << calculator.slews()[u2_a] << std::endl;

    timing::elmore elmore(tree, c0);
    elmore.update();

    timing::elmore_second_moment beta(tree, elmore);
    auto step_slew_u1_a = boost::units::sqrt( (beta.at(u1_a)+beta.at(u1_a)) - boost::units::pow<2>(elmore.at(u1_a)));
    auto step_slew_u2_a = boost::units::sqrt( (beta.at(u2_a)+beta.at(u2_a)) - boost::units::pow<2>(elmore.at(u2_a)));


    std::cout << "elmore delay to tap node " << tree.capacitor_name(u1_a) << " = " << elmore.at(u1_a) << std::endl;
    std::cout << "elmore to tap node " << tree.capacitor_name(u2_a) << " = " << elmore.at(u2_a) << std::endl;
    std::cout << "slew to tap node " << tree.capacitor_name(u1_a) << " = " << sqrt( pow<2>(s_calculator(tree.lumped())) + pow<2>(step_slew_u1_a) ) << std::endl;
    std::cout << "slew to tap node " << tree.capacitor_name(u1_a) << " = " << sqrt( pow<2>(s_calculator(tree.lumped())) + pow<2>(step_slew_u2_a) ) << std::endl;

}

