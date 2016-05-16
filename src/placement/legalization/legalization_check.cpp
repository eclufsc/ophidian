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

#include "legalization_check.h"

namespace ophidian {
    namespace placement {
        namespace legalization {

            bool legalization_check::check_legality() {
                bool legality = check_cell_overlaps();
                return legality;
            }

            bool legalization_check::check_cell_overlaps() {
                rtree cell_boxes_rtree;
                for (auto cell : m_placement->netlist().cell_system()) {
                    auto cell_geometry = m_placement->cell_geometry(cell.first);
                    for (auto cell_polygon : cell_geometry) {
                        box cell_box;
                        boost::geometry::envelope(cell_polygon, cell_box);
                        cell_box.max_corner().x(cell_box.max_corner().x()-1);
                        cell_box.max_corner().y(cell_box.max_corner().y()-1);

                        std::vector<rtree_node> intersecting_nodes;
                        cell_boxes_rtree.query(boost::geometry::index::intersects(cell_box), std::back_inserter(intersecting_nodes));
                        if (intersecting_nodes.empty()) {
                            cell_boxes_rtree.insert(std::make_pair(cell_box, cell.first));
                        } else {
                            return false;
                        }
                    }
                }
                return true;
            }
        }
    }
}
