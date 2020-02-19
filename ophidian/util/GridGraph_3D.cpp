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

#include "GridGraph_3D.h"
#include <cmath>
#include <iostream>

namespace ophidian::util
{

GridGraph_3D::GridGraph_3D(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z, GridGraph_3D::capacity_type capacity_edge, GridGraph_3D::cost_type edge_cost):
    m_width(x), m_height(y), m_depth(z), m_nodes(boost::extents[x][y][z]), m_nodes_indexes(m_graph), m_edge_capacity(m_graph, capacity_edge), m_edge_cost(m_graph ,edge_cost)
{
    //creating graph
    m_graph.reserveNode(x*y*z);
    if(z == 0)
        m_graph.reserveEdge(2*x*y-x-y);
    else
        m_graph.reserveEdge(2*x*y*z);

    for (index_type x_it = 0; x_it < x; ++x_it) {
        for (index_type y_it = 0; y_it < y; ++y_it) {
            for (index_type z_it = 0; z_it < z; ++z_it) {
                auto node = m_graph.addNode();
                m_nodes[x_it][y_it][z_it] = node;
                m_nodes_indexes[node] = node_position_type(x_it, y_it, z_it);
            }
        }
    }

    auto s = m_nodes[0][0][0];
    auto t = m_nodes[0][0][0];
    edge_type edge;
    for (index_type x_it = 0; x_it < x; ++x_it) {
        for (index_type y_it = 0; y_it < y; ++y_it) {
            for (index_type z_it = 0; z_it < z; ++z_it) {
                s = m_nodes[x_it][y_it][z_it];
                //north (x,y,z) -> (x,y+1,z)
                if(y_it +1 < m_height){
                    t = m_nodes[x_it][y_it+1][z_it];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
                //south (x,y,z) -> (x,y-1,z)
                if(y_it -1 >= 0){
                    t = m_nodes[x_it][y_it-1][z_it];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
                //east (x,y,z) -> (x+1,y,z)
                if(x_it +1 < m_width){
                    t = m_nodes[x_it+1][y_it][z_it];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
                //west (x,y,z) -> (x-1,y,z)
                if(x_it -1 >= 0){
                    t = m_nodes[x_it-1][y_it][z_it];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
                //up (x,y,z) -> (x,y,z+1)
                if(z_it +1 < m_depth){
                    t = m_nodes[x_it][y_it][z_it+1];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
                //down (x,y,z) -> (x,y,z-1)
                if(z_it -1 >= 0){
                    t = m_nodes[x_it][y_it][z_it-1];
                    if(lemon::findEdge(m_graph,s,t) == lemon::INVALID){
                        edge = m_graph.addEdge(s,t);
                        m_edge_capacity[edge] = capacity_edge;
                        m_edge_cost[edge] = edge_cost;
                    }
                }
            }
        }
    }
}

GridGraph_3D::node_type GridGraph_3D::node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z) const {
    if(!(x < m_width && x >=0 && y < m_height && y >=0 && z < m_depth && z >=0 )){
        // throw ophidian::util::exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x][y][z];
}

GridGraph_3D::node_position_type GridGraph_3D::position(GridGraph_3D::node_type node)
{
    return m_nodes_indexes[node];
}

GridGraph_3D::cost_type GridGraph_3D::node_distance_x(GridGraph_3D::node_type u, GridGraph_3D::node_type v)
{
    auto position_u = m_nodes_indexes[u];
    auto position_v = m_nodes_indexes[v];
    return cost_type(std::abs(position_u.get<0>() - position_v.get<0>()));
}

GridGraph_3D::cost_type GridGraph_3D::node_distance_y(GridGraph_3D::node_type u, GridGraph_3D::node_type v)
{
    auto position_u = m_nodes_indexes[u];
    auto position_v = m_nodes_indexes[v];
    return cost_type(std::abs(position_u.get<1>() - position_v.get<1>()));
}

GridGraph_3D::cost_type GridGraph_3D::node_distance_z(GridGraph_3D::node_type u, GridGraph_3D::node_type v)
{
    auto position_u = m_nodes_indexes[u];
    auto position_v = m_nodes_indexes[v];
    return cost_type(std::abs(position_u.get<2>() - position_v.get<2>()));
}

GridGraph_3D::node_type GridGraph_3D::north_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return north_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::north_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width && y < m_height-1 && z < m_depth)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x][y+1][z];
}

GridGraph_3D::node_type GridGraph_3D::south_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return south_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::south_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width && y < m_height && y > 0 && z < m_depth)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x][y-1][z];
}

GridGraph_3D::node_type GridGraph_3D::east_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return east_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::east_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width-1 && y < m_height && z < m_depth)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x+1][y][z];
}

GridGraph_3D::node_type GridGraph_3D::west_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return west_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::west_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width && x > 0 && y < m_height && z < m_depth)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x-1][y][z];
}

GridGraph_3D::node_type GridGraph_3D::up_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return up_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::up_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width && y < m_height && z < m_depth-1)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x][y][z+1];
}

GridGraph_3D::node_type GridGraph_3D::down_node(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    return down_node(position.get<0>(), position.get<1>(), position.get<2>());
}

GridGraph_3D::node_type GridGraph_3D::down_node(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z)
{
    if(!(x < m_width && y < m_height && z < m_depth && z > 0)){
        // throw exceptions::BoundariNode();
        return lemon::INVALID;
    }
    return m_nodes[x][y][z-1];
}

GridGraph_3D::edge_type GridGraph_3D::edge(GridGraph_3D::node_type source, GridGraph_3D::node_type target){
    return lemon::findEdge(m_graph,source,target);
}

GridGraph_3D::node_type GridGraph_3D::u(GridGraph_3D::edge_type edge){
    return m_graph.u(edge);
}

GridGraph_3D::node_type GridGraph_3D::v(GridGraph_3D::edge_type edge){
    return m_graph.v(edge);
}

GridGraph_3D::capacity_type GridGraph_3D::capacity(GridGraph_3D::edge_type edge){
    return m_edge_capacity[edge];
}

GridGraph_3D::edge_type GridGraph_3D::north_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<1>() < m_height-1 )){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()][position.get<1>()+1][position.get<2>()];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::north_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width && y < m_height-1 && z < m_depth)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x][y+1][z]);
}

GridGraph_3D::edge_type GridGraph_3D::south_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<1>() > 0 )){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()][position.get<1>()-1][position.get<2>()];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::south_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width && y < m_height && y > 0 && z < m_depth)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x][y-1][z]);
}

GridGraph_3D::edge_type GridGraph_3D::east_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<0>() < m_width-1 )){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()+1][position.get<1>()][position.get<2>()];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::east_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width-1 && y < m_height && z < m_depth)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x+1][y][z]);
}

GridGraph_3D::edge_type GridGraph_3D::west_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<0>() > 0 )){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()-1][position.get<1>()][position.get<2>()];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::west_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width && x > 0 && y < m_height && z < m_depth)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x-1][y][z]);
}

GridGraph_3D::edge_type GridGraph_3D::up_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<2>() < m_depth-1)){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()][position.get<1>()][position.get<2>()+1];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::up_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width && y < m_height && z < m_depth-1)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x][y][z+1]);
}

GridGraph_3D::edge_type GridGraph_3D::down_edge(GridGraph_3D::node_type node){
    auto position = m_nodes_indexes[node];
    if(!( position.get<2>() > 0)){
        return lemon::INVALID;
    }
    auto target = m_nodes[position.get<0>()][position.get<1>()][position.get<2>()-1];
    return lemon::findEdge(m_graph, node, target);
}

GridGraph_3D::edge_type GridGraph_3D::down_edge(GridGraph_3D::index_type x, GridGraph_3D::index_type y, GridGraph_3D::index_type z){
    if(!(x < m_width && y < m_height && z < m_depth && z > 0)){
        return lemon::INVALID;
    }
    return lemon::findEdge(m_graph, m_nodes[x][y][z], m_nodes[x][y][z-1]);
}

void GridGraph_3D::neighbors(GridGraph_3D::node_container_type & neighbors, GridGraph_3D::node_type node){
    neighbors.clear();
    neighbors.reserve(6); //max neighbors   
    for (graph_type::IncEdgeIt e(m_graph, node); e != lemon::INVALID; ++e) {
        auto  oppositeNode = m_graph.oppositeNode(node, e) ;
        neighbors.push_back(oppositeNode);
    }
}

GridGraph_3D::node_container_type GridGraph_3D::neighbors_around_node(GridGraph_3D::node_type node){
    node_container_type neighbors;
    neighbors.reserve(8);
    auto pos = m_nodes_indexes[node];
    for(auto x = std::max(0, pos.get<0>()-1); x <= std::min(x+1, m_width); x++)
    {
        for(auto y = std::max(0, pos.get<1>()-1); y <= std::min(y+1, m_height); y++)
        {
            if(x != pos.get<0>() || y != pos.get<1>())
                continue;
            neighbors.push_back(m_nodes[x][y][pos.get<2>()]);
        }
    }
    return neighbors;
}

void GridGraph_3D::edge_cost(const GridGraph_3D::edge_type edge, const GridGraph_3D::cost_type cost){
    if( edge != lemon::INVALID)
        m_edge_cost[edge] = cost;
}

void GridGraph_3D::edge_cost(GridGraph_3D::node_type u, GridGraph_3D::node_type v, const GridGraph_3D::cost_type cost){
    auto edge = lemon::findEdge(m_graph,u,v);
    if( edge != lemon::INVALID)
        m_edge_cost[edge] = cost;
}

GridGraph_3D::cost_type GridGraph_3D::edge_cost(GridGraph_3D::node_type u, GridGraph_3D::node_type v){
    auto edge = lemon::findEdge(m_graph,u,v);
    if( edge != lemon::INVALID){
        return m_edge_cost[edge];
    }else{
        return std::numeric_limits<cost_type>::max();
    }
}

GridGraph_3D::index_type GridGraph_3D::width(){
    return m_width;
}

GridGraph_3D::index_type GridGraph_3D::height(){
    return m_height;
}

GridGraph_3D::index_type GridGraph_3D::depth(){
    return m_depth;
}


} // namespace
