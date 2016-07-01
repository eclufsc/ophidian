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


#include "method_of_means_and_medians.h"

namespace ophidian {
namespace clock_tree_synthesis {
method_of_means_and_medians::method_of_means_and_medians()
{

}

method_of_means_and_medians::~method_of_means_and_medians()
{

}

void method_of_means_and_medians::build_topology(std::vector<point>::iterator positions_begin, std::vector<point>::iterator positions_end, clock_topology *clock_topology)
{
    std::size_t size = std::distance(positions_begin, positions_end);
    if (size <= 1) {
        return;
    }

    point center_of_mass{0.0, 0.0};
    for (auto position = positions_begin; position != positions_end; ++position) {
        center_of_mass.x(center_of_mass.x() + positions_begin->x());
        center_of_mass.y(center_of_mass.y() + positions_begin->y());
    }
    center_of_mass.x(center_of_mass.x() / size);
    center_of_mass.y(center_of_mass.y() / size);

    return;
}
}
}


