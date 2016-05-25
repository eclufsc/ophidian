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
                bool legality = check_boundaries() && check_alignment() && check_cell_overlaps();
                return legality;
            }

            bool legalization_check::check_cell_overlaps() {
                rtree cell_boxes_rtree;
                for (auto cell : m_placement->netlist().cell_system()) {
                    auto cell_geometry = m_placement->cell_geometry(cell);
                    for (auto cell_polygon : cell_geometry) {
                        box cell_box;
                        boost::geometry::envelope(cell_polygon, cell_box);
                        cell_box.max_corner().x(cell_box.max_corner().x()-1);
                        cell_box.max_corner().y(cell_box.max_corner().y()-1);

                        std::vector<rtree_node> intersecting_nodes;
                        cell_boxes_rtree.query(boost::geometry::index::intersects(cell_box), std::back_inserter(intersecting_nodes));
                        if (intersecting_nodes.empty()) {
                            cell_boxes_rtree.insert(std::make_pair(cell_box, cell));
                        } else {
                            return false;
                        }
                    }
                }
                return true;
            }

            bool legalization_check::check_alignment() {
                for (auto cell_position : m_placement->cell_properties().positions()) {
                    try {
                        auto row = m_floorplan->find_row(cell_position);
                        point row_origin = m_floorplan->row_origin(row);
                        double site_width = m_floorplan->site_dimensions(m_floorplan->row_site(row)).x();
                        if ((cell_position.y() != row_origin.y()) || ((int)cell_position.x() % (int)site_width != 0)) {
                            return false;
                        }
                    } catch (floorplan::row_not_found) {
                        return false;
                    }
                }
                return true;
            }

            bool legalization_check::check_boundaries() {
                box chip_area(m_floorplan->chip_origin(), m_floorplan->chip_boundaries());
                for (auto cell_geometry : m_placement->cell_properties().geometries()) {
                    box cell_boundaries;
                    boost::geometry::envelope(cell_geometry, cell_boundaries);
                    for (auto cell_polygon : cell_geometry) {
                        for (auto cell_point : cell_polygon.outer()) {
                            if (!boost::geometry::intersects(cell_point, chip_area)) {
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
        }
    }
}
