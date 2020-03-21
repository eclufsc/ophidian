#ifndef OPHIDIAN_ROUTING_CONSTRAINTS_H
#define OPHIDIAN_ROUTING_CONSTRAINTS_H

#include <ophidian/entity_system/Property.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/routing/Library.h>
#include <ophidian/parser/ICCAD2020.h>

namespace ophidian::routing
{
    class RoutingConstraints
    {
    public:
        template <class K, class V> using map_type  = std::unordered_map<K,V>;
        using index_type                            = int;
        using demand_type                           = int;
        using layer_type                            = Library::layer_type;
        using net_type                              = ophidian::circuit::Net;
        using macro_name_type                       = std::string;
        using layer_name_type                       = std::string;
        using same_grid_key_type                    = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using adj_grid_key_type                     = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using same_grid_map                         = map_type<same_grid_key_type, demand_type>;
        using adj_grid_map                          = map_type<adj_grid_key_type, demand_type>;

        // Constructors
        //! Construct Netlist
        RoutingConstraints() = delete;

        //! coppy constructor
        RoutingConstraints(const RoutingConstraints &) = delete;
        RoutingConstraints & operator =(const RoutingConstraints &) = delete;

        //! Move Constructor
        RoutingConstraints(RoutingConstraints &&) = delete;
        RoutingConstraints& operator=(RoutingConstraints &&) = delete;

        RoutingConstraints(const ophidian::circuit::Netlist & netlist);

        //Element access
        demand_type ndf_constraint(index_type x, index_type y, index_type z);

        //if there is no constraints, it returns the first metal layer
        layer_type min_net_layer(net_type net);

        //order of parameter does not matter
        demand_type extra_demand_same_grid(macro_name_type m1, macro_name_type m2, layer_name_type l_name);

        //order of parameter does not matter
        demand_type extra_demand_adj_grid(macro_name_type m1, macro_name_type m2, layer_name_type l_name);

        //Modifiers
        void resize_ndf_constraints(std::size_t size_x, std::size_t size_y, std::size_t size_z);
        void set_ndf_constraint(index_type x, index_type y, index_type z, demand_type val);
        void set_min_net_layer(net_type net, layer_type layer);
        void set_same_grid_extra_demand(macro_name_type m1_name, macro_name_type m2_name, layer_name_type l_name, demand_type d);
        void set_adj_grid_extra_demand(macro_name_type m1_name, macro_name_type m2_name, layer_name_type l_name, demand_type d);
    private:
        entity_system::Property<net_type, layer_type> m_min_net_layer;
        std::size_t m_size_x, m_size_y, m_size_z;//It needs to be lazy initialized because of lazy initialization of GCellGraph
        std::vector<demand_type> m_ndf_constraint;//maps a gcell_index to a ndf routing constraint
        same_grid_map m_extra_demand_same_grid_map;
        adj_grid_map m_extra_demand_adj_grid_map;
    };
}
#endif // ROUTING_CONSTRAINTS_H
