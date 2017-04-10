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

#include "Placement.h"

namespace ophidian {
namespace placement {

Placement::Placement(const circuit::Netlist &netlist): 
    cellLocations_(netlist.makeProperty<util::Location>(circuit::Cell())),
    inputLocations_(netlist.makeProperty<util::Location>(circuit::Input())),
    outputLocations_(netlist.makeProperty<util::Location>(circuit::Output()))
    { }

Placement::~Placement()
{

}

void Placement::placeCell(const circuit::Cell & cell, const util::Location & location)
{
    cellLocations_[cell] = location;
}

void Placement::placeInputPad(const circuit::Input &input, const util::Location &location)
{
    inputLocations_[input] = location;
}

util::Location Placement::inputPadLocation(const circuit::Input &input) const
{
    return inputLocations_[input];
}

void Placement::placeOutputPad(const circuit::Output &output, const util::Location &location)
{
    outputLocations_[output] = location;
}

util::Location Placement::outputPadLocation(const circuit::Output &output) const
{
    return outputLocations_[output];
}


} //namespace placement

} //namespace ophidian
