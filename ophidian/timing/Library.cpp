/*
 * Copyright 2017 Ophidian
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


#include "Library.h"

namespace ophidian
{
namespace timing
{

Library::Library(const liberty_type & liberty, standard_cells_type & stdCells, timing_arcs_type & arcs, bool early) :
    m_rise_delays(arcs.makeProperty<LUT>()),
    m_fall_delays(arcs.makeProperty<LUT>()),
    m_rise_slews(arcs.makeProperty<LUT>()),
    m_fall_slews(arcs.makeProperty<LUT>()),
    m_timing_senses(arcs.makeProperty<unateness_type>()),
    m_timing_types(arcs.makeProperty<timing_edge_type>()),
    m_pin_capacitance(stdCells.makeProperty<capacitance_unit_type>(std_pin_entity_type())),
    m_clock(stdCells.makeProperty<bool>(std_pin_entity_type())),
    m_sequential(stdCells.makeProperty<bool>(std_cell_entity_type()))
{
    for (auto cell : liberty.cells)
    {
        for (auto pin : cell.pins)
        {
            for (auto tmg : pin.timing)
            {
                if (!early && tmg.timingType == timing_edge_type::HOLD_RISING ||
                     early && tmg.timingType == timing_edge_type::SETUP_RISING)
                    continue;

                auto relatedPin = cell.find(tmg.relatedPin);
                std::string nameFromPin = cell.name+":"+relatedPin.name;
                std::string nameToPin = cell.name+":"+pin.name;

                auto arc = arcs.add(nameFromPin+"->"+nameToPin);
                arcs.from(arc, stdCells.find(std_pin_entity_type(), nameFromPin));
                arcs.to(arc, stdCells.find(std_pin_entity_type(), nameToPin));
                m_timing_senses[arc] = tmg.timingSense;
                m_timing_types[arc] = tmg.timingType;
                auto temporario = tmg.find(ParserLUT::CELL_RISE);
                m_rise_delays[arc] = LUT(temporario);
                m_fall_delays[arc] = LUT(tmg.find(ParserLUT::CELL_FALL));
                m_rise_slews[arc] = LUT(tmg.find(pin.pinDirection == liberty_type::Pin::INPUT?
                                                   ParserLUT::RISE_CONSTRAINT : ParserLUT::RISE_TRANSITION));
                m_fall_slews[arc] = LUT(tmg.find(pin.pinDirection == liberty_type::Pin::INPUT?
                                                   ParserLUT::FALL_CONSTRAINT : ParserLUT::FALL_TRANSITION));
            }

            std_pin_entity_type stdPin = stdCells.find(std_pin_entity_type(), cell.name+":"+pin.name);
            m_pin_capacitance[stdPin] = capacitance_unit_type(pin.capacitance);
            m_clock[stdPin] = pin.clock;
        }

        m_sequential[stdCells.find(std_cell_entity_type(), cell.name)] = cell.sequential;
    }
}

Library::time_unit_type Library::computeRiseDelay(const timing_arc_entity_type & arc, const capacitance_unit_type rv, const time_unit_type cv) const
{
    return m_rise_delays[arc].compute(rv, cv);
}

Library::time_unit_type Library::computeFallDelay(const timing_arc_entity_type & arc, const capacitance_unit_type rv, const time_unit_type cv) const
{
    return m_fall_delays[arc].compute(rv, cv);
}

Library::time_unit_type Library::computeRiseSlews(const timing_arc_entity_type & arc, const capacitance_unit_type rv, const time_unit_type cv) const
{
    return m_rise_slews[arc].compute(rv, cv);
}

Library::time_unit_type Library::computeFallSlews(const timing_arc_entity_type & arc, const capacitance_unit_type rv, const time_unit_type cv) const
{
    return m_fall_slews[arc].compute(rv, cv);
}

Library::unateness_type Library::unateness(const timing_arc_entity_type & arc) const
{
    return m_timing_senses[arc];
}

Library::timing_edge_type Library::type(const timing_arc_entity_type & arc) const
{
    return m_timing_types[arc];
}

Library::capacitance_unit_type Library::capacitance(const std_pin_entity_type & pin) const
{
    return m_pin_capacitance[pin];
}

bool Library::pinClock(const std_pin_entity_type & pin) const
{
    return m_clock[pin];
}

bool Library::cellSequential(const std_cell_entity_type & cell) const
{
    return m_sequential[cell];
}

} // namespace timing
} // namespace ophidian
