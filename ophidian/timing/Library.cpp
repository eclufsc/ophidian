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

Library::Library(const parser::Liberty & liberty, standard_cell::StandardCells & stdCells, TimingArcs & arcs) :
    mRiseDelays(arcs.makeProperty<LUT>()),
    mFallDelays(arcs.makeProperty<LUT>()),
    mRiseSlews(arcs.makeProperty<LUT>()),
    mFallSlews(arcs.makeProperty<LUT>()),
    mTimingSenses(arcs.makeProperty<unateness_t>()),
    mTimingTypes(arcs.makeProperty<timing_type_t>())
{
    for (auto cell : liberty.cells)
    {
        if (cell.sequential) {

        } else {
            for (auto out : cell.outputs())
                for (auto tmg : out.timing)
                    for (auto in : cell.inputs())
                        if (in.name == tmg.relatedPin) {
                            auto arc = arcs.add(cell.name+":"+in.name+"->"+cell.name+":"+out.name);
                            arcs.from(arc, stdCells.find(standard_cell::Pin(), cell.name+":"+in.name));
                            arcs.to(arc, stdCells.find(standard_cell::Pin(), cell.name+":"+out.name));
                            mTimingSenses[arc] = tmg.timingSense;
                            mTimingTypes[arc] = tmg.timingType;
                            mRiseDelays[arc] = tmg.find(parser::Liberty::LUT::CELL_RISE);
                            mFallDelays[arc] = tmg.find(parser::Liberty::LUT::CELL_FALL);
                            mRiseSlews[arc] = tmg.find(parser::Liberty::LUT::RISE_TRANSITION);
                            mFallSlews[arc] = tmg.find(parser::Liberty::LUT::FALL_TRANSITION);
                        }
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
