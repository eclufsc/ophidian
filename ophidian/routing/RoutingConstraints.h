#ifndef OPHIDIAN_ROUTING_CONSTRAINTS_H
#define OPHIDIAN_ROUTING_CONSTRAINTS_H

#include <ophidian/entity_system/Property.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/routing/Library.h>
#include <ophidian/parser/ICCAD2020.h>

#include "boost/tuple/tuple.hpp"

namespace ophidian::routing
{
    class RoutingConstraints
    {
    public:
        struct tuple_hash
        {
            template <class T1, class T2, class T3>
            std::size_t operator() (const std::tuple<T1, T2, T3> &tuple) const
            {
                return std::hash<T1>()(std::get<0>(tuple)) ^ std::hash<T2>()(std::get<1>(tuple)) ^ std::hash<T2>()(std::get<2>(tuple));
            }
        };

        struct extra_demand_rule {
            std::string macro1;
            std::string macro2;
            std::string layer;
            int demand;
        };


        template <class K, class V> using map_type  = std::unordered_map<K,V>;
        using index_type                            = int;
        using demand_type                           = extra_demand_rule;
        using layer_type                            = Library::layer_type;
        using net_type                              = ophidian::circuit::Net;
        using macro_name_type                       = std::string;
        using layer_name_type                       = std::string;
        using same_grid_key_type                    = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using adj_grid_key_type                     = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using same_grid_map                         = map_type<same_grid_key_type, demand_type>;
        using adj_grid_map                          = map_type<adj_grid_key_type, demand_type>;

        using ndf_index_type                        = std::tuple<index_type, index_type, index_type>;
        // using tuple_hash_type                       = tuple_hash<index_type, index_type, index_type>;
        using ndf_map_type                          = std::unordered_map<ndf_index_type, int, tuple_hash>;

        // Constructors
        //! Construct Netlist
        RoutingConstraints() = delete;

        //! coppy constructor
        RoutingConstraints(const RoutingConstraints &) = delete;
        RoutingConstraints & operator =(const RoutingConstraints &) = delete;

        //! Move Constructor
        RoutingConstraints(RoutingConstraints &&) = delete;
        RoutingConstraints& operator=(RoutingConstraints &&) = delete;

        RoutingConstraints(const ophidian::circuit::Netlist & netlist, const ophidian::routing::Library & library);

        //Element access
        int ndf_constraint(index_type x, index_type y, index_type z) const;

        same_grid_map::const_iterator begin_same_grid() const;
        same_grid_map::const_iterator end_same_grid() const;

        adj_grid_map::const_iterator begin_adj_grid() const;
        adj_grid_map::const_iterator end_adj_grid() const;

        //if there is no constraints, it returns the first metal layer
        layer_type min_net_layer(net_type net);

        //order of parameter does not matter
        demand_type extra_demand_same_grid(macro_name_type m1, macro_name_type m2, layer_name_type l_name);

        //order of parameter does not matter
        demand_type extra_demand_adj_grid(macro_name_type m1, macro_name_type m2, layer_name_type l_name);

        unsigned int max_cell_movement();

        //Modifiers
        void resize_ndf_constraints(std::size_t size_x, std::size_t size_y, std::size_t size_z);
        void set_max_cell_movement(unsigned int max_move);
        void set_ndf_constraint(index_type x, index_type y, index_type z, int val);
        void set_min_net_layer(net_type net, layer_type layer);
        void set_same_grid_extra_demand(macro_name_type m1_name, macro_name_type m2_name, layer_name_type l_name, int d);
        void set_adj_grid_extra_demand(macro_name_type m1_name, macro_name_type m2_name, layer_name_type l_name, int d);
    private:
        entity_system::Property<net_type, layer_type> m_min_net_layer;
        std::size_t m_size_x, m_size_y, m_size_z;//It needs to be lazy initialized because of lazy initialization of GCellGraph
        // std::vector<demand_type> m_ndf_constraint;//maps a gcell_index to a ndf routing constraint
        ndf_map_type m_ndf_constraint; //maps a gcell_index to a ndf routing constraint
        same_grid_map m_extra_demand_same_grid_map;
        adj_grid_map m_extra_demand_adj_grid_map;
        unsigned int m_max_cell_move;

    };
}
#endif // ROUTING_CONSTRAINTS_H
