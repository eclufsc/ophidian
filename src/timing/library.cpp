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

#include "library.h"

namespace ophidian {
namespace timing {

library::library(library_timing_arcs * tarcs, ophidian::standard_cell::standard_cells * std_cells) :
		m_std_cells(*std_cells), m_tarcs(*tarcs) {
	std_cells->register_pin_property(&m_pin_capacitance);
	m_tarcs.register_property(&m_rise_delays);
	m_tarcs.register_property(&m_rise_slews);
	m_tarcs.register_property(&m_fall_delays);
	m_tarcs.register_property(&m_fall_slews);
	m_tarcs.register_property(&m_timing_senses);
    m_tarcs.register_property(&m_timing_types);
    m_tarcs.register_property(&m_arc2setup_rise);
    m_tarcs.register_property(&m_arc2hold_rise);
    m_tarcs.register_property(&m_arc2setup_fall);
    m_tarcs.register_property(&m_arc2hold_fall);
}

library::~library() {
}

void library::pin_capacitance(entity_system::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance) {
    m_pin_capacitance[m_std_cells.pin_system().lookup(pin)] = capacitance;
}

void library::hold_rise_create(entity_system::entity arc, const library::TestLUT &lut)
{
    m_arc2hold_rise[ m_tarcs.system().lookup(arc) ] = m_tests.size();
    m_tests.push_back(lut);
}

void library::setup_rise_create(entity_system::entity arc, const library::TestLUT &lut)
{
    m_arc2setup_rise[ m_tarcs.system().lookup(arc) ] = m_tests.size();
    m_tests.push_back(lut);
}


void library::hold_fall_create(entity_system::entity arc, const library::TestLUT &lut)
{
    m_arc2hold_fall[ m_tarcs.system().lookup(arc) ] = m_tests.size();
    m_tests.push_back(lut);
}

void library::setup_fall_create(entity_system::entity arc, const library::TestLUT &lut)
{
    m_arc2setup_fall[ m_tarcs.system().lookup(arc) ] = m_tests.size();
    m_tests.push_back(lut);
}

void library::timing_arc_rise_slew(entity_system::entity arc, const LUT& lut) {
	m_rise_slews[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_fall_slew(entity_system::entity arc, const LUT& lut) {
	m_fall_slews[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_rise_delay(entity_system::entity arc, const LUT& lut) {
	m_rise_delays[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_fall_delay(entity_system::entity arc, const LUT& lut) {
	m_fall_delays[m_tarcs.system().lookup(arc)] = lut;
}

entity_system::entity library::cell_create(std::string name) {
	return m_std_cells.cell_create(name);
}

entity_system::entity library::pin_create(entity_system::entity cell, std::string name) {
    return m_std_cells.pin_create(cell, name);
}

void library::cell_sequential(entity_system::entity cell, bool sequential)
{
    m_std_cells.cell_sequential(cell, sequential);
}

void library::pin_clock_input(entity_system::entity pin, bool clock_input)
{
    m_std_cells.pin_clock_input(pin, clock_input);
}

void library::timing_arc_timing_sense(entity_system::entity arc, unateness timing_sense) {
    m_timing_senses[m_tarcs.system().lookup(arc)] = timing_sense;
}

void library::timing_arc_timing_type(entity_system::entity arc, timing_arc_types type)
{
    m_timing_types[m_tarcs.system().lookup(arc)] = type;
}

} /* namespace timing */
} /* namespace ophidian */

