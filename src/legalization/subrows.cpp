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

#include "subrows.h"

namespace ophidian {
namespace legalization {

void subrows::begin(entity_system::entity subrow, double begin) {
    m_begin[m_system.lookup(subrow)] = begin;
}

void subrows::end(entity_system::entity subrow, double end) {
    m_end[m_system.lookup(subrow)] = end;
}

void subrows::row(entity_system::entity subrow, entity_system::entity row) {
    m_row[m_system.lookup(subrow)] = row;
}

void subrows::create_subrows(floorplan::floorplan *floorplan, placement::placement * placement) {
    for (auto row : floorplan->rows_system()) {
        auto subrow = m_system.create();
        this->begin(subrow, floorplan->row_origin(row).x());
        this->end(subrow, floorplan->row_origin(row).x() + floorplan->row_dimensions(row).x() - 1);
        this->row(subrow, row);
        point subrow_min_corner = floorplan->row_origin(row);
        point subrow_max_corner(floorplan->row_origin(row).x() + floorplan->row_dimensions(row).x() - 1, floorplan->row_origin(row).y() + floorplan->row_dimensions(row).y() - 1);
        box subrow_box(subrow_min_corner, subrow_max_corner);
        subrows_rtree.insert(std::make_pair(subrow_box, subrow));
    }

    for (auto cell : placement->netlist().cell_system()) {
        if (placement->cell_fixed(cell)) {
            auto cell_geometry = placement->cell_geometry(cell);
            for (auto cell_polygon : cell_geometry) {
                box cell_rectangle;
                boost::geometry::envelope(cell_polygon, cell_rectangle);
                std::vector<rtree_node> intersecting_nodes;
                subrows_rtree.query(boost::geometry::index::intersects(cell_rectangle), std::back_inserter(intersecting_nodes));
                for (auto node : intersecting_nodes) {
                    auto subrow = node.second;
                    auto row = this->row(subrow);

                    auto left_subrow = m_system.create();
                    this->begin(left_subrow, this->begin(subrow));
                    this->end(left_subrow, cell_rectangle.min_corner().x() - 1);
                    this->row(left_subrow, row);
                    point left_subrow_min_corner(this->begin(left_subrow), floorplan->row_origin(row).y());
                    point left_subrow_max_corner(cell_rectangle.min_corner().x() - 1, floorplan->row_origin(row).y() + floorplan->row_dimensions(row).y() - 1);
                    box left_subrow_box(left_subrow_min_corner, left_subrow_max_corner);
                    subrows_rtree.insert(std::make_pair(left_subrow_box, left_subrow));

                    auto right_subrow = m_system.create();
                    this->begin(right_subrow, cell_rectangle.max_corner().x());
                    this->end(right_subrow, this->end(subrow));
                    this->row(right_subrow, this->row(subrow));
                    point right_subrow_min_corner(cell_rectangle.max_corner().x(), floorplan->row_origin(row).y());
                    point right_subrow_max_corner(this->end(subrow), floorplan->row_origin(row).y() + floorplan->row_dimensions(row).y() - 1);
                    box right_subrow_box(right_subrow_min_corner, right_subrow_max_corner);
                    subrows_rtree.insert(std::make_pair(right_subrow_box, right_subrow));

                    m_system.destroy(subrow);
                    subrows_rtree.remove(node);
                }
            }
        }
    }
}

entity_system::entity subrows::find_subrow(point coordinate) {
    std::vector<rtree_node> intersecting_nodes;
    subrows_rtree.query(boost::geometry::index::intersects(coordinate), std::back_inserter(intersecting_nodes));
    if (intersecting_nodes.empty()) {
        throw subrow_not_found();
    }
    assert(intersecting_nodes.size() == 1);
    return intersecting_nodes.front().second;
}

std::vector<entity_system::entity> subrows::find_closest_subrows(point coordinate, unsigned number_of_rows)
{
    std::vector<rtree_node> closest_nodes;
    subrows_rtree.query(boost::geometry::index::nearest(coordinate, number_of_rows), std::back_inserter(closest_nodes));
    std::vector<entity_system::entity> subrows;
    subrows.reserve(closest_nodes.size());
    for (auto node : closest_nodes) {
        subrows.push_back(node.second);
    }
    return subrows;
}
}
}
