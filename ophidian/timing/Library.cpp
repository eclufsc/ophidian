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

using namespace standard_cell;

namespace timing
{

Library::Library(StandardCells& stdCells) :
    pinCapacitances_(stdCells.makeProperty<util::Capacitance>(Pin{})),
    timing_(),
    pinTiming_(stdCells.makeComposition<Timing>(Pin{}, timing_)),
    relatedPin_(timing_),
    timingSense_(timing_),
    lookupTables_(entity_system::Property<Timing, std::map<std::string, LookupTable> >(timing_)),
    timingType_(timing_, TimingType::COMBINATIONAL)
{

}

Library::~Library()
{

}

void Library::capacitance(const Pin& pin, util::Capacitance value)
{
    pinCapacitances_[pin] = value;
}

util::Capacitance Library::capacitance(const Pin& pin) const
{
    return pinCapacitances_[pin];
}

entity_system::Association<standard_cell::Pin, Library::Timing>::Parts Library::timing(const Pin& pin) const
{
    return pinTiming_.parts(pin);
}

Library::Timing Library::add(Library::Timing, const Pin& pin)
{
    auto timing = timing_.add();
    pinTiming_.addAssociation(pin, timing);
    return timing;
}

void Library::relatedPin(const Library::Timing& timing, const Pin& pin)
{
    relatedPin_[timing] = pin;
}

Pin Library::relatedPin(const Library::Timing& timing) const
{
    return relatedPin_[timing];
}

void Library::timingSense(const Library::Timing& timing, Unateness sense)
{
    timingSense_[timing] = sense;
}

Unateness Library::timingSense(const Library::Timing& timing) const
{
    return timingSense_[timing];
}

std::map<std::string, Library::LookupTable>& Library::lookupTables(const Library::Timing& timing)
{
    return lookupTables_[timing];
}

void Library::timingType(const Library::Timing &timing, TimingType type)
{
    timingType_[timing] = type;
}

TimingType Library::timingType(const Library::Timing &timing) const
{
    return timingType_[timing];
}

}
}
