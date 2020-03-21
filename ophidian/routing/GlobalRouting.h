#ifndef OPHIDIAN_GLOBAL_ROUTING_H
#define OPHIDIAN_GLOBAL_ROUTING_H

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
        using layer_type                = Library::layer_type;
        using net_type                  = ophidian::circuit::Net;
        using gcell_graph_type          = GCellGraph;
        using gcell_graph_ptr_type      = std::shared_ptr<gcell_graph_type>;
        using index_type                = GCellGraph::index_type;

        using net_segment_view_type  = entity_system::Association<net_type, gr_segment_type>::Parts;

        // Constructors
        //! Construct Netlist
        GlobalRouting() = delete;

        //! coppy constructor
        GlobalRouting(const GlobalRouting &) = delete;
        GlobalRouting & operator =(const GlobalRouting &) = delete;

        //! Move Constructor
        GlobalRouting(GlobalRouting &&) = delete;
        GlobalRouting& operator=(GlobalRouting &&) = delete;

        GlobalRouting(const ophidian::circuit::Netlist & netlist);

        // Element access
        net_segment_view_type segments(const net_type& net) const;
        net_type net(const gr_segment_type& segment) const;
        const segment_geometry_type& box(const gr_segment_type& segment) const;
        const layer_type layer(const gr_segment_type& segment) const;

        void create_gcell_graph(unit_container_type x, unit_container_type y, index_type z, GCellGraph::scalar_container_type capacities);
        void create_gcell_graph(const ophidian::routing::Library & library, unit_container_type x, unit_container_type y, index_type z);
        gcell_graph_ptr_type gcell_graph();

        // Iterators
        segment_container_type::const_iterator begin_segment() const noexcept;
        segment_container_type::const_iterator end_segment() const noexcept;

        // Capacity
        segment_container_type::size_type size_segment() const noexcept;

        // Modifiers
        gr_segment_type add_segment(const segment_geometry_type & box, const layer_type & layer, const net_type & net);

        template <typename Value>
        entity_system::Property<gr_segment_type, Value> makeProperty(gr_segment_type) const
        {
            return entity_system::Property<gr_segment_type, Value>(m_gr_segments);
        }

        entity_system::EntitySystem<gr_segment_type>::NotifierType * notifier(gr_segment_type) const;

    private:
        entity_system::EntitySystem<gr_segment_type>            m_gr_segments;
        entity_system::Property<gr_segment_type, segment_geometry_type>      m_gr_segment_box;
        entity_system::Property<gr_segment_type, layer_type>    m_gr_segment_layers;

        entity_system::Aggregation<net_type, gr_segment_type>   m_net_to_gr_segment;

        gcell_graph_ptr_type m_gcell_graph;
    };
}

#endif // LIBRARY_H
