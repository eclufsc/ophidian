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

Library::Library(const parser::Liberty & liberty, standard_cell::StandardCells & stdCells, TimingArcs & arcs, bool early) :
    mRiseDelays(arcs.makeProperty<LUT>()),
    mFallDelays(arcs.makeProperty<LUT>()),
    mRiseSlews(arcs.makeProperty<LUT>()),
    mFallSlews(arcs.makeProperty<LUT>()),
    mTimingSenses(arcs.makeProperty<unateness_t>()),
    mTimingTypes(arcs.makeProperty<timing_type_t>()),
    mPinCapacitance(stdCells.makeProperty<util::farad_t>(standard_cell::Pin())),
    mClock(stdCells.makeProperty<bool>(standard_cell::Pin())),
    mSequential(stdCells.makeProperty<bool>(standard_cell::Cell()))
{
    for (auto cell : liberty.cells)
    {
        for (auto pin : cell.pins)
        {
            for (auto tmg : pin.timing)
            {
                if (!early && tmg.timingType == timing_type_t::HOLD_RISING ||
                     early && tmg.timingType == timing_type_t::SETUP_RISING)
                    continue;

                auto relatedPin = cell.find(tmg.relatedPin);
                std::string nameFromPin = cell.name+":"+relatedPin.name;
                std::string nameToPin = cell.name+":"+pin.name;

                auto arc = arcs.add(nameFromPin+"->"+nameToPin);
                arcs.from(arc, stdCells.find(standard_cell::Pin(), nameFromPin));
                arcs.to(arc, stdCells.find(standard_cell::Pin(), nameToPin));
                mTimingSenses[arc] = tmg.timingSense;
                mTimingTypes[arc] = tmg.timingType;
                auto temporario = tmg.find(ParserLUT::CELL_RISE);
                mRiseDelays[arc] = LUT(temporario);
                mFallDelays[arc] = LUT(tmg.find(ParserLUT::CELL_FALL));
                mRiseSlews[arc] = LUT(tmg.find(pin.pinDirection == parser::Liberty::Pin::INPUT?
                                                   ParserLUT::RISE_CONSTRAINT : ParserLUT::RISE_TRANSITION));
                mFallSlews[arc] = LUT(tmg.find(pin.pinDirection == parser::Liberty::Pin::INPUT?
                                                   ParserLUT::FALL_CONSTRAINT : ParserLUT::FALL_TRANSITION));
            }

            standard_cell::Pin stdPin = stdCells.find(standard_cell::Pin(), cell.name+":"+pin.name);
            mPinCapacitance[stdPin] = util::farad_t(pin.capacitance);
            mClock[stdPin] = pin.clock;
        }

        mSequential[stdCells.find(standard_cell::Cell(), cell.name)] = cell.sequential;
    }
}

util::second_t Library::computeRiseDelay(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const
{
    return mRiseDelays[arc].compute(rv, cv);
}

util::second_t Library::computeFallDelay(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const
{
    return mFallDelays[arc].compute(rv, cv);
}

util::second_t Library::computeRiseSlews(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const
{
    return mRiseSlews[arc].compute(rv, cv);
}

util::second_t Library::computeFallSlews(const TimingArc & arc, const util::farad_t rv, const util::second_t cv) const
{
    return mFallSlews[arc].compute(rv, cv);
}

Library::unateness_t Library::unateness(const TimingArc & arc) const
{
    return mTimingSenses[arc];
}

Library::timing_type_t Library::type(const TimingArc & arc) const
{
    return mTimingTypes[arc];
}

util::farad_t Library::capacitance(const standard_cell::Pin & pin) const
{
    return mPinCapacitance[pin];
}

bool Library::pinClock(const standard_cell::Pin & pin) const
{
    return mClock[pin];
}

bool Library::cellSequential(const standard_cell::Cell & cell) const
{
    return mSequential[cell];
}

} // namespace timing
} // namespace ophidian
