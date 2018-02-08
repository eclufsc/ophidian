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

Library::Library(std::shared_ptr<parser::Liberty> & liberty, const standard_cell::StandardCells & stdCells, TimingArcs & arcs) :
    mRiseDelays(arcs.makeProperty<LUT>()),
    mFallDelays(arcs.makeProperty<LUT>()),
    mRiseSlews(arcs.makeProperty<LUT>()),
    mFallSlews(arcs.makeProperty<LUT>()),
    mTimingSenses(arcs.makeProperty<unateness_t>())
{

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


} // namespace timing
} // namespace ophidian
