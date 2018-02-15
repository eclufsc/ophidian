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
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/TimingArcs.h>

namespace ophidian
{
namespace timing
{

using unateness_t = parser::Liberty::Timing::unateness;
using timing_type_t = parser::Liberty::Timing::type;

class Library
{
    using LUT = parser::Liberty::LUT;

public:
    Library(const parser::Liberty & liberty, standard_cell::StandardCells & stdCells, TimingArcs & arcs);

    double computeRiseDelay(const Arc & arc, double rv, double cv);
    double computeFallDelay(const Arc & arc, double rv, double cv);
    double computeRiseSlews(const Arc & arc, double rv, double cv);
    double computeFallSlews(const Arc & arc, double rv, double cv);
    unateness_t unateness(const Arc & arc);
    timing_type_t type(const Arc & arc);

private:
    entity_system::Property<Arc, LUT> mRiseDelays;
    entity_system::Property<Arc, LUT> mFallDelays;
    entity_system::Property<Arc, LUT> mRiseSlews;
    entity_system::Property<Arc, LUT> mFallSlews;
    entity_system::Property<Arc, unateness_t> mTimingSenses;
    entity_system::Property<Arc, timing_type_t> mTimingTypes;

};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_LIBRARY_H
