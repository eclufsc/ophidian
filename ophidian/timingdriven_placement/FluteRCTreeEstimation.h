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

#ifndef OPHIDIAN_TIMINGDRIVEN_PLACEMENT_FLUTERCTREEESTIMATION_H
#define OPHIDIAN_TIMINGDRIVEN_PLACEMENT_FLUTERCTREEESTIMATION_H

#include <ophidian/util/Units.h>
#include <ophidian/placement/PlacementMapping.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/Library.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/timingdriven_placement/RCTree.h>
#include <ophidian/geometry/Distance.h>

namespace ophidian
{
namespace timingdriven_placement
{

class FluteRCTreeBuilder
{
public:
    using SourceRCTree = RCTree::Capacitor;

    FluteRCTreeBuilder(double maxSegmentLength = 100.0);
    virtual ~FluteRCTreeBuilder();

    void maxSegmentLength(double maximum);
    void resistancePerMicrons(const util::ohm_t resistance);
    void capacitancePerMicron(const util::farad_t capacitance);

    SourceRCTree build(const placement::PlacementMapping & placementMap,
                       const circuit::LibraryMapping & libraryMap,
                       const circuit::Netlist & netlist,
                       const timing::Library & library,
                       const parser::Lef & lef,
                       const circuit::Net net,
                       RCTree & rctree,
                       const circuit::Pin source);

private:
    struct Parameters
    {
        double mMaxSegmentLength;
        util::ohm_t mResistancePerMicron;
        util::farad_t mCapacitancePerMicron;
    };

    Parameters mParameters;
};

}   // namespace timingdriven_placement
}   // ophidian

#endif // OPHIDIAN_TIMINGDRIVEN_PLACEMENT_FLUTERCTREEESTIMATION_H
