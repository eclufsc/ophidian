#ifndef OPHIDIAN_GLOBAL_ROUTING_H
#define OPHIDIAN_GLOBAL_ROUTING_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/util/Units.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/routing/Library.h>

namespace ophidian::routing
{
    class Region :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class GlobalRouting
    {
    public:
        using scalar_type           = int;
        using unit_type             = util::database_unit_t;
        using region_type           = Region;
        using region_container_type = std::vector<region_type>;
        using region_geometry_type  = geometry::Box<unit_type>;
        using layer_type            = Library::layer_type;
        using net_type              = ophidian::circuit::Net;

        using net_region_view_type  = entity_system::Association<net_type, region_type>::Parts;

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
        net_region_view_type regions(const net_type& net) const;
        net_type net(const region_type& region) const;
        const region_geometry_type& box(const region_type& region) const;
        const layer_type layer(const region_type& region) const;

        // Iterators
        region_container_type::const_iterator begin_region() const noexcept;
        region_container_type::const_iterator end_region() const noexcept;

        // Capacity
        region_container_type::size_type size_region() const noexcept;

        // Modifiers
        region_type add_region(const region_geometry_type & box, const layer_type & layer, const net_type & net);

        template <typename Value>
        entity_system::Property<region_type, Value> makeProperty(region_type) const
        {
            return entity_system::Property<region_type, Value>(m_regions);
        }

        entity_system::EntitySystem<region_type>::NotifierType * notifier(region_type) const;

    private:
        entity_system::EntitySystem<region_type>            m_regions;
        entity_system::Property<region_type, region_geometry_type>      m_region_box;
        entity_system::Property<region_type, layer_type>    m_region_layers;

        entity_system::Aggregation<net_type, region_type>   m_net_to_regions;
    };
}

#endif // LIBRARY_H
