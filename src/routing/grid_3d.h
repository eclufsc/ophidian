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

#ifndef OPHIDIAN_SRC_ROUTING_GRID_3D_H
#define OPHIDIAN_SRC_ROUTING_GRID_3D_H

#include <lemon/grid_graph.h>

#include <memory>
#include <assert.h>

namespace ophidian{
/// Namespace describing routing entities and basic routing interface.
namespace routing{

/// Grid_2d class.
/**
* This class provides the basic interface for manipulating a 2D routing grid including edges capacities and demands.
*/
class grid_2d
{
    friend class grid_3d;

    lemon::GridGraph m_graph;
    lemon::GridGraph::EdgeMap<unsigned> m_edges_capacities;
    lemon::GridGraph::EdgeMap<unsigned> m_edges_demands;
public:
    /// Constructor.
    /**
 * Grid 2d constructor. Initializes the 2D routing grid.
 * \param width number of gcells in the x direction.
 * \param height number of gcells in the y direction.
 */
    grid_2d(int width, int height);
    virtual ~grid_2d();
};

/// Grid_3d class.
/**
    * This class provides the basic interface for manipulating a 3D routing grid including edges capacities and demands.
    * It handles layers in horizontal and vertical directions. This version assumes that even layers are routed horizontally while odd layers are routed vertically.
    * Layers are numbered from 0 to n-1.
    * For a 2D routing, the user can use an unique layers and merge all the capacities in that layer.
    */
class grid_3d
{
    int m_width;
    int m_height;
    int m_depth;
    std::vector<std::unique_ptr<grid_2d> > m_vector_of_grid_2D;
public:
    /// Constructor.
    /**
     * Grid 2d constructor. Initializes the 2D routing grid.
     * \param width number of gcells in the x direction.
     * \param height number of gcells in the y direction.
     * \param depth number of metal layers. Numbered from 0 to n-1
     */
    grid_3d(int width, int height, int depth);
    ~grid_3d();

    /// Get grid width.
    /**
     * Returns the grid width, i.e. number of gcells in the x coordinate.
     * \return Grid width.
     */
    inline int width(){return m_width;}

    /// Get grid height.
    /**
     * Returns the grid height, i.e. number of gcells in the y coordinate.
     * \return Grid height.
     */
    inline int height(){return m_height;}

    /// Get grid depth.
    /**
     * Returns the grid depth, i.e. number of layers.
     * \return Grid depth.
     */
    inline int depth(){return m_depth;}

    /// Reset all edge demands to zero.
    /**
     * Reset all edge demands to zero.
     */
    void reset_edge_demands();

    /// Get capacity of a given horizontal edge.
    /**
     * Returns capacity of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Capacity of the horizontal edge.
     */
    inline unsigned get_horizontal_edge_capacity(int x, int y, int z)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        return grid_2d.m_edges_capacities[arc];
    }

    /// Get capacity of a given vertical edge.
    /**
     * Returns capacity of a given vertical edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Capacity of the vertical edge.
     */
    inline unsigned get_vertical_edge_capacity(int x, int y, int z)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        return grid_2d.m_edges_capacities[arc];
    }

    /// Set the capacity of a given horizontal edge.
    /**
     * Set the capacity of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \param capacity Capacity of the edge, i.e. number of routing tracks that can pass through the edge.
     */
    inline void set_horizontal_edge_capacity(int x, int y, int z, unsigned capacity)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        grid_2d.m_edges_capacities[arc] = capacity;
    }

    /// Set the capacity of a given vertical edge.
    /**
     * Set the capacity of a given verticak edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \param capacity Capacity of the edge, i.e. number of routing tracks that can pass through the edge.
     */
    inline void set_vertical_edge_capacity(int x, int y, int z, unsigned capacity)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        grid_2d.m_edges_capacities[arc] = capacity;
    }

    /// Get the demand of a given horizontal edge.
    /**
     * Returns demand of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Demand of the horizontal edge.
     */
    inline unsigned get_horizontal_edge_demand(int x, int y, int z)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        return grid_2d.m_edges_demands[arc];
    }

    /// Get the demand of a given vertical edge.
    /**
     * Returns demand of a given vertical edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Demand of the vertical edge.
     */
    unsigned get_vertical_edge_demand(int x, int y, int z)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        return grid_2d.m_edges_demands[arc];
    }

    /// Set the demand of a given horizontal edge.
    /**
     * Set the demand of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \param demand Demand of the edge, i.e. number of routing tracks passing through the edge.
     */
    inline void set_horizontal_edge_demand(int x, int y, int z, unsigned demand)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        grid_2d.m_edges_demands[arc] = demand;
    }

    /// Set the demand of a given vertical edge.
    /**
     * Set the demand of a given vertical edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \param demand Demand of the edge, i.e. number of routing tracks passing through the edge.
     */
    inline void set_vertical_edge_demand(int x, int y, int z, unsigned demand)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        grid_2d.m_edges_demands[arc] = demand;
    }

    /// Increment the demand of a given horizontal edge.
    /**
     * Increment by 1 the demand of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     */
    inline void increment_horizontal_edge_demand(int x, int y, int z)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        grid_2d.m_edges_demands[arc] += 1;
    }

    /// Increment the demand of a given vertical edge.
    /**
     * Increment by 1 the demand of a given vertical edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     */
    inline void increment_vertical_edge_demand(int x, int y, int z)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        grid_2d.m_edges_demands[arc] += 1;
    }

    /// Get the utilization gap of a given horizontal edge.
    /**
     * Returns the utilization gap of a given horizontal edge connecting gcells at coordinates x and x+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Utilization gap of the edge, i.e. (capacity - demand).
     */
    inline int get_horizontal_edge_utilization_gap(int x, int y, int z)
    {
        assert(x < m_width-1); //there is no boundary edge
        assert(y < m_height);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.right(grid_2d.m_graph(x, y));
        auto capacity = grid_2d.m_edges_capacities[arc];
        auto demand = grid_2d.m_edges_demands[arc];
        return capacity - demand;
    }

    /// Get the utilization gap of a given vertical edge.
    /**
     * Returns the utilization gap of a given vertical edge connecting gcells at coordinates y and y+1.
     * \param x x coordinate.
     * \param y y coordinate.
     * \param z z coordinate, i.e. layer.
     * \return Utilization gap of the edge, i.e. (capacity - demand).
     */
    inline int get_vertical_edge_utilization_gap(int x, int y, int z)
    {
        assert(x < m_width); //there is no boundary edge
        assert(y < m_height-1);
        assert(z < m_depth);

        auto & grid_2d = *m_vector_of_grid_2D.at(z);
        auto arc = grid_2d.m_graph.up(grid_2d.m_graph(x, y));
        auto capacity = grid_2d.m_edges_capacities[arc];
        auto demand = grid_2d.m_edges_demands[arc];
        return capacity - demand;
    }

    /// Get the maximum and total edge overflow in the 3d grid.
    /**
     * Returns the maximum and total edge overflow in the 3d grid.
     * \param max_overflow Reference to an integer that will contain the maximum edge overflow in the 3d grid.
     * \param total_overflow Reference to an integer that will contain the total edge overflow in the 3d grid.
     */
    void get_max_and_total_edges_overflow(int & max_overflow, int & total_overflow);
};

class edge_overflow_calculator{
protected:
    grid_3d & m_grid_3d;
public:
    edge_overflow_calculator(grid_3d & grid_3d);
    virtual ~edge_overflow_calculator();
    void get_max_and_total_edges_overflow(int & max_overflow, int & total_overflow);
};

}  /* namespace routing */
}  /* namespace ophidian */

#endif // OPHIDIAN_SRC_ROUTING_GRID_3D_H
