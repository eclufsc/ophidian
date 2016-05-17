/*
 * Copyright 2016 Ophidian
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

#include "stwl.h"
#include "hpwl.h"
#include "flute.h"

namespace ophidian {
    namespace interconnection {
        double stwl(const std::vector<geometry::point<double> >&points) {
            readLUT();
            if(points.size() <= 3)
                return hpwl(points);
            std::vector<unsigned> X(points.size());
            std::vector<unsigned> Y(points.size());
            X.resize(0); Y.resize(0);
            for (auto & p : points) {
                X.push_back(p.x());
                Y.push_back(p.y());
            }
            auto tree = flute(points.size(), X.data(), Y.data(), ACCURACY);
            delete tree.branch;
            return tree.length;
        }
    }
}
