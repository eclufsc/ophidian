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

#ifndef OPHIDIAN_TIMING_LIBRARY_H
#define OPHIDIAN_TIMING_LIBRARY_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/util/Units.h>
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/timing/LookupTable.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/TimingArcs.h>

namespace ophidian
{
namespace timing
{

class Library
{
public:
    using liberty_type           = parser::Liberty;
    using standard_cells_type    = standard_cell::StandardCells;
    using timing_arcs_type       = TimingArcs;

    using time_unit_type         = util::second_t;
    using capacitance_unit_type  = util::farad_t;
    using unateness_type         = liberty_type::Timing::unateness;
    using timing_edge_type       = liberty_type::Timing::type;

    using pin_entity_type        = circuit::Pin;
    using std_pin_entity_type    = standard_cell::Pin;
    using std_cell_entity_type   = standard_cell::Cell;
    using timing_arc_entity_type = TimingArc;

    template <class V> using property_timing_arc_type = entity_system::Property<timing_arc_entity_type, V>;
    template <class V> using property_std_pin_type    = entity_system::Property<std_pin_entity_type, V>;
    template <class V> using property_std_cell_type   = entity_system::Property<std_cell_entity_type, V>;

    Library() = default;

    //! Library Constructor
    /*!
       \brief Constructs a Library with properties.
       \param liberty The parser with the LUT tables.
       \param stdCells The Standard Cells's system of the circuit.
       \param arcs The Timing Arcs' system of the circuit.
     */
    Library(const liberty_type & liberty,
            standard_cells_type & stdCells,
            timing_arcs_type & arcs,
            bool early = false);

    //! Compute Rise Delay
    /*!
       \brief Calculating the rise delay of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Delay's value.
     */
    time_unit_type computeRiseDelay(const timing_arc_entity_type & arc,
                                    const capacitance_unit_type rv,
                                    const time_unit_type cv) const;

    //! Compute Fall Delay
    /*!
       \brief Calculating the fall delay of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Delay's value.
     */
    time_unit_type computeFallDelay(const timing_arc_entity_type & arc,
                                    const capacitance_unit_type rv,
                                    const time_unit_type cv) const;

    //! Compute Rise Slews
    /*!
       \brief Calculating the rise slews of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Slew's value.
     */
    time_unit_type computeRiseSlews(const timing_arc_entity_type & arc,
                                    const capacitance_unit_type rv,
                                    const time_unit_type cv) const;

    //! Compute Fall Slews
    /*!
       \brief Calculating the fall slews of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Slew's value.
     */
    time_unit_type computeFallSlews(const timing_arc_entity_type & arc,
                                    const capacitance_unit_type rv,
                                    const time_unit_type cv) const;

    //! Unateness Getter
    /*!
       \brief Get the unateness of an arc.
       \param arc The Timing TimingArc.
       \return TimingArc's unateness.
     */
    unateness_type unateness(const timing_arc_entity_type & arc) const;

    //! type Getter
    /*!
       \brief Get the timing type of an arc.
       \param arc The Timing TimingArc.
       \return TimingArc's timing type.
     */
    timing_edge_type type(const timing_arc_entity_type & arc) const;

    //! Capacitance Getter
    /*!
       \brief Get the capacitance of a pin.
       \param pin The Standard Pin.
       \return Pin's capacitance.
     */
    capacitance_unit_type capacitance(const std_pin_entity_type & pin) const;

    //! Check clock pin
    /*!
       \brief Checks whether a standard pin is a clock pin.
       \param pin The Standard Pin.
       \return True only if it is clock pin.
     */
    bool pinClock(const std_pin_entity_type & pin) const;

    //! Check sequential cell
    /*!
       \brief Checks whether a standard cell is sequential.
       \param cell The Standard Cell.
       \return True only if it is sequential.
     */
    bool cellSequential(const std_cell_entity_type & cell) const;

private:
    using ParserLUT = liberty_type::LUT;
    using LUT = LookupTable<capacitance_unit_type, time_unit_type, time_unit_type>;

    property_timing_arc_type<LUT>                m_rise_delays;
    property_timing_arc_type<LUT>                m_fall_delays;
    property_timing_arc_type<LUT>                m_rise_slews;
    property_timing_arc_type<LUT>                m_fall_slews;
    property_timing_arc_type<unateness_type>     m_timing_senses;
    property_timing_arc_type<timing_edge_type>   m_timing_types;
    property_std_pin_type<capacitance_unit_type> m_pin_capacitance;
    property_std_pin_type<bool>                  m_clock;
    property_std_cell_type<bool>                 m_sequential;

};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_LIBRARY_H
