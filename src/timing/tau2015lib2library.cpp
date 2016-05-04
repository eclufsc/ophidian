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

#include "tau2015lib2library.h"
#include "lookup_table.h"

using namespace boost::units;
using namespace boost::units::si;

namespace ophidian {
namespace timing {


timing::lookup_table<quantity<si::capacitance>, quantity<si::time>, quantity<si::time> > create_lut(const double values[3])
{

    timing::lookup_table<quantity<si::capacitance>, quantity<si::time>, quantity<si::time> >LUT(2,2);
    LUT.row_value(0, quantity<capacitance>(0.0*farads));
    LUT.row_value(1, quantity<capacitance>(48.0*femto*farads));
    LUT.column_value(0, quantity<si::time>(0.0*seconds));
    LUT.column_value(1, quantity<si::time>(300.0*pico*seconds));
    for(std::size_t i{0}; i < LUT.row_count(); ++i)
    {
        for(std::size_t j{0}; j < LUT.row_count(); ++j)
        {
            quantity<si::time> value(values[0]*seconds + quantity<resistance>(values[1]*ohms)*LUT.row_value(i) + values[2]*LUT.column_value(j) );
            LUT.at(i,j, value);
        }
    }
    return LUT;
}


timing::lookup_table<quantity<si::time>, quantity<si::time>, quantity<si::time> > create_test_lut(const double values[3]) {
    timing::lookup_table<quantity<si::time>, quantity<si::time>, quantity<si::time> > LUT(2,2);

    LUT.row_value(0, quantity<si::time>(0.0*seconds));
    LUT.row_value(1, quantity<si::time>(300.0*pico*seconds));
    LUT.column_value(0, quantity<si::time>(0.0*seconds));
    LUT.column_value(1, quantity<si::time>(300.0*pico*seconds));

    for(std::size_t i{0}; i < LUT.row_count(); ++i)
    {
        for(std::size_t j{0}; j < LUT.row_count(); ++j)
        {
            quantity<si::time> value(values[0]*seconds + values[1]*LUT.row_value(i) + values[2]*LUT.column_value(j) );
            LUT.at(i,j, value);
        }
    }
    return LUT;
}

void tau2015lib2library(const parsing::tau2014::linear_library &tau, library &lib)
{
    for(auto & cell_pair : tau.cells())
    {
        const parsing::tau2014::linear_library::cell & cell = cell_pair.second;
        auto cell_entity = lib.cell_create(cell.name);
        for(auto pin : cell.input_pins)
        {
            auto pin_entity = lib.pin_create(cell_entity, pin.name);
            lib.pin_capacitance(pin_entity, quantity<capacitance>(pin.rise_capacitance*farads));
            if(pin.clock)
            {
                lib.cell_sequential(cell_entity, true);
                lib.pin_clock_input(pin_entity, true);
            }
            lib.pin_direction(pin_entity, standard_cell::pin_directions::INPUT);
        }

        for(auto pin : cell.output_pins)
        {
            auto pin_entity = lib.pin_create(cell_entity, pin.name);
            lib.pin_direction(pin_entity, standard_cell::pin_directions::OUTPUT);
        }

        for(auto & input_pin_arcs : cell.arcs)
        {
            for(auto & arc_pair : input_pin_arcs.second)
            {
                const parsing::tau2014::linear_library::arc & arc = arc_pair.second;
                const parsing::tau2014::linear_library::pin & from = cell.input_pins[arc.from];
                const parsing::tau2014::linear_library::pin & to = cell.output_pins[arc.to];
                auto timing_arc_entity = lib.timing_arc_create(lib.pin_create(cell_entity, from.name), lib.pin_create(cell_entity, to.name));
                lib.timing_arc_fall_delay( timing_arc_entity, create_lut(arc.fall_delay) );
                lib.timing_arc_rise_delay( timing_arc_entity, create_lut(arc.rise_delay) );
                lib.timing_arc_fall_slew( timing_arc_entity, create_lut(arc.fall_slew) );
                lib.timing_arc_rise_slew( timing_arc_entity, create_lut(arc.rise_slew) );
            }
        }

        for(const parsing::tau2014::linear_library::test & hold_test : cell.hold)
        {
            const parsing::tau2014::linear_library::pin & from = cell.input_pins[hold_test.from];
            const parsing::tau2014::linear_library::pin & to = cell.input_pins[hold_test.to];
            lib.hold_fall_create(lib.timing_arc_create(lib.pin_create(cell_entity, from.name), lib.pin_create(cell_entity, to.name)), create_test_lut(hold_test.fall));
            lib.hold_rise_create(lib.timing_arc_create(lib.pin_create(cell_entity, from.name), lib.pin_create(cell_entity, to.name)), create_test_lut(hold_test.rise));
        }

        for(const parsing::tau2014::linear_library::test & setup_test : cell.setup)
        {
            const parsing::tau2014::linear_library::pin & from = cell.input_pins[setup_test.from];
            const parsing::tau2014::linear_library::pin & to = cell.input_pins[setup_test.to];
            lib.setup_fall_create(lib.timing_arc_create(lib.pin_create(cell_entity, from.name), lib.pin_create(cell_entity, to.name)), create_test_lut(setup_test.fall));
            lib.setup_rise_create(lib.timing_arc_create(lib.pin_create(cell_entity, from.name), lib.pin_create(cell_entity, to.name)), create_test_lut(setup_test.rise));
        }
    }
}


}
}
