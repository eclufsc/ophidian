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

#ifndef OPHIDIAN_TIMING_TIMINGGRAPHBUILDER_H
#define OPHIDIAN_TIMING_TIMINGGRAPHBUILDER_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingArcs.h>
#include <ophidian/timing/TimingGraph.h>
#include <ophidian/parser/SDCParser.h>

namespace ophidian
{
namespace timing
{

class TimingGraphBuilder
{
public:
    TimingGraphBuilder();

    std::shared_ptr<TimingGraph> build(const circuit::Netlist & netlist,
                                       const standard_cell::StandardCells & stdCells,
                                       const circuit::LibraryMapping & libraryMapping,
                                       const TimingArcs & timingArcs,
                                       const Library & timingLibrary,
                                       const parser::DesignConstraints & dc);
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_TIMINGGRAPHBUILDER_H
