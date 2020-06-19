#ifndef OPHIDIAN_UTIL_GRID_GRAPH_3D_H
#define OPHIDIAN_UTIL_GRID_GRAPH_3D_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/util/UtilException.h>
// #include <ophidian/routing/Library.h>
// #include <ophidian/floorplan/Floorplan.h>
#include <ophidian/util/Units.h>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include <boost/geometry.hpp>
#include <lemon/list_graph.h>
#include <assert.h>
#include <string>

namespace ophidian::util
{

    class GridGraph_3D
    {
    public:
        using unit_type = database_unit_t;
        using dbu_point_type = LocationDbu;
        using index_type = int;
        using capacity_type = int;
        using cost_type  = int; 
        using node_position_type = boost::geometry::model::point<index_type, 3, boost::geometry::cs::cartesian>;
        using graph_type = lemon::ListGraph;
        using node_type = lemon::ListGraph::Node;
        using edge_type = lemon::ListGraph::Edge;

        using node_container_type = std::vector<node_type>;
        template <class T> using node_map_type = graph_type::NodeMap<T>;
        template <class T> using edge_map_type = graph_type::EdgeMap<T>;
        using matrix_type = boost::multi_array<node_type, 3, std::allocator<node_type> >;

        GridGraph_3D() = delete;
        GridGraph_3D( GridGraph_3D& g) = delete;

        GridGraph_3D(index_type x, index_type y, index_type z, capacity_type capacity_edge = 1, cost_type edge_cost = 1 );

        node_type node(index_type x, index_type y, index_type z) const;
        node_position_type position(node_type node) const;

        cost_type node_distance_x(node_type u, node_type v);
        cost_type node_distance_y(node_type u, node_type v);
        cost_type node_distance_z(node_type u, node_type v);

        node_type north_node(node_type node);
        node_type north_node(index_type x, index_type y, index_type z);

        node_type south_node(node_type node);
        node_type south_node(index_type x, index_type y, index_type z);

        node_type east_node(node_type node);
        node_type east_node(index_type x, index_type y, index_type z);

        node_type west_node(node_type node);
        node_type west_node(index_type x, index_type y, index_type z);

        node_type up_node(node_type node);
        node_type up_node(index_type x, index_type y, index_type z);

        node_type down_node(node_type node);
        node_type down_node(index_type x, index_type y, index_type z);

        edge_type edge(node_type source, node_type target);
        node_type u(edge_type edge);
        node_type v(edge_type edge);
        capacity_type capacity(edge_type edge);

        edge_type north_edge(node_type node);
        edge_type north_edge(index_type x, index_type y, index_type z);

        edge_type south_edge(node_type node);
        edge_type south_edge(index_type x, index_type y, index_type z);

        edge_type east_edge(node_type node);
        edge_type east_edge(index_type x, index_type y, index_type z);

        edge_type west_edge(node_type node);
        edge_type west_edge(index_type x, index_type y, index_type z);

        edge_type up_edge(node_type node);
        edge_type up_edge(index_type x, index_type y, index_type z);

        edge_type down_edge(node_type node);
        edge_type down_edge(index_type x, index_type y, index_type z);

        void neighbors(node_container_type & neighbors, node_type node);
        node_container_type neighbors_around_node(node_type node);

        void edge_cost(const edge_type edge, const cost_type cost);
        void edge_cost(node_type u, node_type v, const cost_type cost);
        cost_type edge_cost(node_type u, node_type v);

        index_type width();
        index_type height();
        index_type depth();
        
    protected:
        graph_type m_graph;
    private:
        index_type m_width;
        index_type m_height;
        index_type m_depth;
        matrix_type m_nodes;

        node_map_type<node_position_type> m_nodes_indexes;

        edge_map_type<capacity_type> m_edge_capacity;
        edge_map_type<cost_type> m_edge_cost;
    };

} // namespace


#endif // OPHIDIAN_UTIL_GRID_GRAPH_3D
