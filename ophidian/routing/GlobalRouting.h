#ifndef OPHIDIAN_GLOBAL_ROUTING_H
#define OPHIDIAN_GLOBAL_ROUTING_H

#include <lemon/euler.h>

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/util/Units.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/routing/Library.h>
#include <ophidian/routing/GCellGraph.h>

namespace ophidian::routing
{
    class GRSegment :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class GlobalRouting
    {
    public:
        template <class T> using container_type = std::vector<T>;
        using scalar_type               = int;
        using unit_type                 = util::database_unit_t;
        using unit_container_type       = container_type<unit_type>;
        using gr_segment_type           = GRSegment;
        using segment_container_type    = container_type<gr_segment_type>;
        using segment_geometry_type     = geometry::Box<unit_type>;
        using gcell_type                = GCell;
        using gcell_container_type      = container_type<gcell_type>;
        using layer_type                = Library::layer_type;
        using net_type                  = ophidian::circuit::Net;
        using net_container_type        = container_type<net_type>;
        using gcell_graph_type          = GCellGraph;
        using gcell_graph_ptr_type      = std::shared_ptr<gcell_graph_type>;
        using index_type                = GCellGraph::index_type;

        using net_segment_view_type  = entity_system::Association<net_type, gr_segment_type>::Parts;
        using graph_type                = lemon::ListGraph;
        using node_type                 = graph_type::Node;       
        using edge_type                 = graph_type::Edge;   

        // Constructors
        //! Construct Netlist
        GlobalRouting() = delete;

        //! coppy constructor
        GlobalRouting(const GlobalRouting &) = delete;
        GlobalRouting & operator =(const GlobalRouting &) = delete;

        //! Move Constructor
        GlobalRouting(GlobalRouting &&) = delete;
        GlobalRouting& operator=(GlobalRouting &&) = delete;

        GlobalRouting(const ophidian::circuit::Netlist & netlist, const ophidian::routing::Library & library);

        // Element access
        net_segment_view_type segments(const net_type& net) const;
        net_type net(const gr_segment_type& segment) const;
        const segment_geometry_type& box(const gr_segment_type& segment) const;
        const layer_type layer_start(const gr_segment_type& segment) const;
        const layer_type layer_end(const gr_segment_type& segment) const;
        const gcell_type gcell_start(const gr_segment_type& segment) const;
        const gcell_type gcell_end(const gr_segment_type& segment) const;

        void create_gcell_graph(unit_container_type x, unit_container_type y, index_type z, GCellGraph::scalar_container_type capacities);
        void create_gcell_graph(const ophidian::routing::Library & library, unit_container_type x, unit_container_type y, index_type z);
        gcell_graph_ptr_type gcell_graph();
        gcell_container_type gcells(const net_type& net) const;

        const scalar_type wirelength_in_gcell(const net_type & net) const;
        const scalar_type wirelength_in_gcell(const net_container_type & nets) const;

        // Iterators
        segment_container_type::const_iterator begin_segment() const noexcept;
        segment_container_type::const_iterator end_segment() const noexcept;

        // Capacity
        segment_container_type::size_type size_segment() const noexcept;

        // Modifiers
        gr_segment_type add_segment(const segment_geometry_type & box, const layer_type & layer_start, const layer_type & layer_end, const net_type & net);
        void increase_demand(const net_type& net);
        void decrease_demand(const net_type& net);

        void unroute(const net_type& net);

        template <typename Value>
        entity_system::Property<gr_segment_type, Value> makeProperty(gr_segment_type) const
        {
            return entity_system::Property<gr_segment_type, Value>(m_gr_segments);
        }

        entity_system::EntitySystem<gr_segment_type>::NotifierType * notifier(gr_segment_type) const;

        bool is_connected(const net_type & net, const gcell_container_type & pin_gcells, std::string net_name) const;
        /* Returns if there is overflow in any of the nets in nets_to_check. Overflowed nets are returned in ovfl_nets*/
        bool is_overflow(const net_container_type& nets_to_check, net_container_type& ovfl_nets) const;
        /* Checks entire circuit. Quicker, checks each Gcell only once*/
        bool is_overflow() const;
        
    private:
        void set_gcells(const gr_segment_type& segment);
        void update_gcells_demand(const gr_segment_type & segment, const int delta);

        entity_system::EntitySystem<gr_segment_type>                    m_gr_segments;
        entity_system::Property<gr_segment_type, segment_geometry_type> m_gr_segment_box;
        entity_system::Property<gr_segment_type, layer_type>            m_gr_segment_layers_start;
        entity_system::Property<gr_segment_type, layer_type>            m_gr_segment_layers_end;
        entity_system::Property<gr_segment_type, gcell_type>            m_gr_segment_gcell_start;
        entity_system::Property<gr_segment_type, gcell_type>            m_gr_segment_gcell_end;

        entity_system::Aggregation<net_type, gr_segment_type>           m_net_to_gr_segment;

        gcell_graph_ptr_type                                            m_gcell_graph;

        const Library&                                                  m_library;
    };
}

#endif // LIBRARY_H
