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

#ifndef OPHIDIAN_LEGALIZATION_CHECK_H
#define OPHIDIAN_LEGALIZATION_CHECK_H

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include "placement.h"
#include "floorplan.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            using point = geometry::point<double>;
            using box = geometry::box<point>;
            using rtree_node = std::pair<box, entity::entity>;
            using rtree = boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>>;

            class legalization_check {
                floorplan::floorplan * m_floorplan;
                placement * m_placement;

                bool check_cell_overlaps();

            public:

                legalization_check(floorplan::floorplan *m_floorplan, placement *m_placement) : m_floorplan(
                        m_floorplan), m_placement(m_placement) { }

                bool check_legality();
            };
        }
    }
}


#endif //OPHIDIAN_LEGALIZATION_CHECK_H
