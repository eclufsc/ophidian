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

#include "Flute.h"
#include "SteinerTree.h"
#include <flute.h>
#include <ophidian/geometry/Distance.h>
#include <ophidian/geometry/Operations.h>

namespace ophidian
{
namespace interconnection
{

    Flute::Flute()
    {
        readLUT();
    }

    Flute & Flute::instance()
    {
        static Flute instance;

        return instance;
    }

    Flute::~Flute()
    {
    }

    std::unique_ptr<SteinerTree> Flute::singleSegment(
        const Flute::Point & p1,
        const Flute::Point & p2)
    {
        auto steiner = SteinerTree::create();

        steiner->add(steiner->add(p1), steiner->add(p2));

        return std::move(steiner);
    }

    std::unique_ptr<SteinerTree> Flute::trivialSteinerTree(const Flute::Point & p)
    {
        auto steiner = SteinerTree::create();

        steiner->add(p);

        return std::move(steiner);
    }

    std::unique_ptr<SteinerTree> Flute::callFlute(
        const std::vector<unsigned> & X,
        const std::vector<unsigned> & Y,
        const Flute::Point & offset)
    {
        Tree tree = flute(
            static_cast<int32_t>(X.size()),
            const_cast<unsigned *>(X.data()),
            const_cast<unsigned *>(Y.data()),
            ACCURACY);
        auto      steiner = SteinerTree::create();
        const int numBranches = 2 * tree.deg - 2;

        for(int i = 0; i < numBranches; ++i)
        {
            const Branch & branch = tree.branch[i];
            int n = tree.branch[i].n;
            if(i == n) {
                continue;
            }
            const Branch & branchN = tree.branch[n];

            Flute::Point u{
                dbu_t{static_cast<double>(branch.x)},
                dbu_t{static_cast<double>(branch.y)}
            };
            Flute::Point v{
                dbu_t{static_cast<double>(branchN.x)}, 
                dbu_t{static_cast<double>(branchN.y)}
            };

            auto translate = [&offset](Flute::Point & p) {
                                 p.x(p.x() - offset.x());
                                 p.y(p.y() - offset.y());
                             };

            translate(u);
            translate(v);

            if(geometry::ManhattanDistance(u, v) > dbu_t{std::numeric_limits<double>::epsilon()}) {
                steiner->add(steiner->add(u), steiner->add(v));
            }
        }
        delete[] tree.branch;

        return std::move(steiner);
    }
}     // namespace interconnection
}     // namespace ophidian
