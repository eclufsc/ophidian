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
    mPinCapacitance(stdCells.makeProperty<double>(standard_cell::Pin()))
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
                mRiseDelays[arc] = tmg.find(LUT::CELL_RISE);
                mFallDelays[arc] = tmg.find(LUT::CELL_FALL);
                mRiseSlews[arc] = tmg.find(pin.pinDirection == parser::Liberty::Pin::INPUT? LUT::RISE_CONSTRAINT : LUT::RISE_TRANSITION);
                mFallSlews[arc] = tmg.find(pin.pinDirection == parser::Liberty::Pin::INPUT? LUT::FALL_CONSTRAINT : LUT::FALL_TRANSITION);
            }

            mPinCapacitance[stdCells.find(standard_cell::Pin(), cell.name+":"+pin.name)] = pin.capacitance;
        }
    }
}

double Library::computeRiseDelay(const Arc & arc, double rv, double cv)
{
    return mRiseDelays[arc].compute(rv, cv);
}

double Library::computeFallDelay(const Arc & arc, double rv, double cv)
{
    return mFallDelays[arc].compute(rv, cv);
}

double Library::computeRiseSlews(const Arc & arc, double rv, double cv)
{
    return mRiseSlews[arc].compute(rv, cv);
}

double Library::computeFallSlews(const Arc & arc, double rv, double cv)
{
    return mFallSlews[arc].compute(rv, cv);
}

unateness_t Library::unateness(const Arc & arc)
{
    return mTimingSenses[arc];
}

timing_type_t Library::type(const Arc & arc)
{
    return mTimingTypes[arc];
}

} // namespace timing
} // namespace ophidian
