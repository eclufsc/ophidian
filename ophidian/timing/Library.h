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
    using unateness_t = ophidian::parser::Liberty::Timing::unateness;
    using timing_type_t = ophidian::parser::Liberty::Timing::type;

    //! Library Constructor
    /*!
       \brief Constructs a Library with properties.
       \param liberty The parser with the LUT tables.
       \param stdCells The Standard Cells's system of the circuit.
       \param arcs The Timing Arcs' system of the circuit.
     */
    Library(const parser::Liberty & liberty,
            standard_cell::StandardCells & stdCells,
            TimingArcs & arcs,
            bool early = false);

    //! Compute Rise Delay
    /*!
       \brief Calculating the rise delay of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Delay's value.
     */
    util::second_t computeRiseDelay(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const;

    //! Compute Fall Delay
    /*!
       \brief Calculating the fall delay of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Delay's value.
     */
    util::second_t computeFallDelay(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const;

    //! Compute Rise Slews
    /*!
       \brief Calculating the rise slews of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Slew's value.
     */
    util::second_t computeRiseSlews(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const;

    //! Compute Fall Slews
    /*!
       \brief Calculating the fall slews of an arc.
       \param arc The Timing TimingArc.
       \param rv Resistence
       \param cv Capacitance
       \return Slew's value.
     */
    util::second_t computeFallSlews(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const;

    //! Unateness Getter
    /*!
       \brief Get the unateness of an arc.
       \param arc The Timing TimingArc.
       \return TimingArc's unateness.
     */
    unateness_t unateness(const TimingArc & arc) const;

    //! type Getter
    /*!
       \brief Get the timing type of an arc.
       \param arc The Timing TimingArc.
       \return TimingArc's timing type.
     */
    timing_type_t type(const TimingArc & arc) const;

    //! Capacitance Getter
    /*!
       \brief Get the capacitance of a pin.
       \param pin The Standard Pin.
       \return Pin's capacitance.
     */
    util::farad_t capacitance(const standard_cell::Pin & pin) const;

    //! Check clock pin
    /*!
       \brief Checks whether a standard pin is a clock pin.
       \param pin The Standard Pin.
       \return True only if it is clock pin.
     */
    bool pinClock(const standard_cell::Pin & pin) const;

    //! Check sequential cell
    /*!
       \brief Checks whether a standard cell is sequential.
       \param cell The Standard Cell.
       \return True only if it is sequential.
     */
    bool cellSequential(const standard_cell::Cell & cell) const;

private:
    using ParserLUT = parser::Liberty::LUT;
    using LUT = LookupTable<util::farad_t, util::second_t, util::second_t>;

    entity_system::Property<TimingArc, LUT>                    mRiseDelays;
    entity_system::Property<TimingArc, LUT>                    mFallDelays;
    entity_system::Property<TimingArc, LUT>                    mRiseSlews;
    entity_system::Property<TimingArc, LUT>                    mFallSlews;
    entity_system::Property<TimingArc, unateness_t>            mTimingSenses;
    entity_system::Property<TimingArc, timing_type_t>          mTimingTypes;
    entity_system::Property<standard_cell::Pin, util::farad_t> mPinCapacitance;
    entity_system::Property<standard_cell::Pin, bool>          mClock;
    entity_system::Property<standard_cell::Cell, bool>         mSequential;

};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_LIBRARY_H
