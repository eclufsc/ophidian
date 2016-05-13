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

#include "grid_3d.h"
#include <limits>

namespace ophidian
{

namespace routing
{

grid_2d::grid_2d(int width, int height) : m_graph(width, height), m_edges_capacities(m_graph), m_edges_demands(m_graph)
{

}

grid_2d::~grid_2d()
{

}

grid_3d::grid_3d(int width, int height, int depth) : m_width(width), m_height(height), m_depth(depth), m_vector_of_grid_2D(m_depth)
{
    for(int i = 0; i < m_depth; ++i)
        m_vector_of_grid_2D.at(i).reset(new grid_2d(width, height));
}

grid_3d::~grid_3d()
{

}

void grid_3d::reset_edge_demands()
{
    for(int z = 0; z < m_depth; ++z)
    {
        for(int y = 0; y < m_height; ++y)
        {
            for(int x = 0; x < m_width-1; ++x)
            {
                set_horizontal_edge_demand(x, y, z, 0);
                set_vertical_edge_demand(x, y, z, 0);
            }
        }
    }
}

edge_overflow_calculator::edge_overflow_calculator(grid_3d & grid_3d) : m_grid_3d(grid_3d)
{

}

edge_overflow_calculator::~edge_overflow_calculator()
{

}

void edge_overflow_calculator::get_max_and_total_edges_overflow(int &max_overflow, int &total_overflow)
{
    max_overflow = std::numeric_limits<int>::max();
    total_overflow = 0;

    for(int z = 0; z < m_grid_3d.depth(); ++z)
    {
        for(int y = 0; y < m_grid_3d.height(); ++y)
        {
            for(int x = 0; x < m_grid_3d.width()-1; ++x)
            {
                int violation = m_grid_3d.get_horizontal_edge_utilization_gap(x, y, z);
                total_overflow += std::min(0, violation);
                max_overflow = std::min(max_overflow, violation);
            }
        }
    }

    for(int z = 0; z < m_grid_3d.depth(); ++z)
    {
        for(int y = 0; y < m_grid_3d.height()-1; ++y)
        {
            for(int x = 0; x < m_grid_3d.width(); ++x)
            {
                int violation = m_grid_3d.get_vertical_edge_utilization_gap(x, y, z);
                total_overflow += std::min(0, violation);
                max_overflow = std::min(max_overflow, violation);
            }
        }
    }

    total_overflow = -total_overflow;
    max_overflow = std::max(-max_overflow, 0);
}

}  /* namespace routing */
}  /* namespace ophidian */

