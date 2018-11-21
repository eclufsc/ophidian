#ifndef OPHIDIAN_ROUTING_LIBRARY_H
#define OPHIDIAN_ROUTING_LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/util/Units.h>
#include <ophidian/util/LookupTable.h>
#include <unordered_map>

namespace ophidian::routing
{
    class Layer :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    enum class LayerType {
        ROUTING, MASTERSLICE, CUT, NA
    };

    enum class LayerDirection {
        HORIZONTAL, VERTICAL, NA
    };

    class Via :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Track :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    enum class TrackOrientation {
        X, Y
    };

    class Library
    {
    public:
        using scalar_type           = int;
        using unit_type             = util::database_unit_t;
        using table_strategy_type   = util::FloorStrategy<unit_type, unit_type, unit_type>;
        using spacing_table_type    = util::LookupTable<unit_type, unit_type, unit_type, table_strategy_type>;
        using spacing_table_content_type = util::TableContents<unit_type, unit_type, unit_type>;

        using layer_type            = Layer;
        using layer_container_type  = entity_system::EntitySystem<layer_type>;

        using layer_type_type       = LayerType;
        using layer_name_type       = std::string;
        using layer_direction_type  = LayerDirection;

        using via_type              = Via;
        using via_container_type    = entity_system::EntitySystem<via_type>;

        using via_name_type         = std::string;
        using via_geometry_type     = geometry::Box<unit_type>;

        using layer_name_to_via_geometry_type = std::unordered_map<layer_name_type, via_geometry_type>;

        using track_type            = Track;
        using track_container_type  = entity_system::EntitySystem<track_type>;

        using track_orientation_type = TrackOrientation;

        // Constructors
        //! Construct Netlist
        Library() = default;

        //! coppy constructor
        Library(const Library &) = delete;
        Library & operator =(const Library &) = delete;

        //! Move Constructor
        Library(Library &&) = delete;
        Library& operator=(Library &&) = delete;

        // Element access
        layer_type find_layer(const layer_name_type& layerName) const;

        via_type find_via(const via_name_type& viaName) const;

        layer_name_type& name(const layer_type& layer);
        const layer_name_type& name(const layer_type& layer) const;

        layer_type_type& type(const layer_type& layer);
        const layer_type_type& type(const layer_type& layer) const;

        layer_direction_type& direction(const layer_type& layer);
        const layer_direction_type& direction(const layer_type& layer) const;

        unit_type& pitch(const layer_type& layer);
        const unit_type& pitch(const layer_type& layer) const;

        unit_type& offset(const layer_type& layer);
        const unit_type& offset(const layer_type& layer) const;

        unit_type& width(const layer_type& layer);
        const unit_type& width(const layer_type& layer) const;

        unit_type& min_width(const layer_type& layer);
        const unit_type& min_width(const layer_type& layer) const;

        unit_type& area(const layer_type& layer);
        const unit_type& area(const layer_type& layer) const;

        unit_type& spacing(const layer_type& layer);
        const unit_type& spacing(const layer_type& layer) const;

        unit_type& EOLspace(const layer_type& layer);
        const unit_type& EOLspace(const layer_type& layer) const;

        unit_type& EOLwidth(const layer_type& layer);
        const unit_type& EOLwidth(const layer_type& layer) const;

        unit_type& EOLwithin(const layer_type& layer);
        const unit_type& EOLwithin(const layer_type& layer) const;

        spacing_table_type& spacing_table(const layer_type& layer);
        const spacing_table_type& spacing_table(const layer_type& layer) const;

        via_name_type& name(const via_type& via);
        const via_name_type& name(const via_type& via) const;

        via_geometry_type& geometry(const via_type& via, const layer_name_type& layer_name);
        const via_geometry_type& geometry(const via_type& via, const layer_name_type& layer_name) const;

        track_orientation_type& orientation(const track_type& track);
        const track_orientation_type& orientation(const track_type& track) const;

        unit_type& start(const track_type& track);
        const unit_type& start(const track_type& track) const;

        scalar_type& number_of_tracks(const track_type& track);
        const scalar_type& number_of_tracks(const track_type& track) const;

        unit_type& space(const track_type& track);
        const unit_type& space(const track_type& track) const;

        layer_type& layer(const track_type& track);
        const layer_type& layer(const track_type& track) const;

        // Iterators
        layer_container_type::const_iterator begin_layer() const noexcept;
        layer_container_type::const_iterator end_layer() const noexcept;

        via_container_type::const_iterator begin_via() const noexcept;
        via_container_type::const_iterator end_via() const noexcept;

        track_container_type::const_iterator begin_track() const noexcept;
        track_container_type::const_iterator end_track() const noexcept;

        // Capacity
        layer_container_type::size_type size_layer() const noexcept;
        via_container_type::size_type size_via() const noexcept;
        track_container_type::size_type size_track() const noexcept;

        // Modifiers
        layer_type add_layer(const layer_name_type& layerName, const layer_type_type& type, const layer_direction_type& direction,
                                      const Library::unit_type& pitch, const Library::unit_type& offset, const Library::unit_type& width,
                                      const Library::unit_type& minWidth, const Library::unit_type& area, const Library::unit_type& spacing,
                                      const Library::unit_type& EOLspace, const Library::unit_type& EOLwidth, const Library::unit_type& EOLwithin,
                                      const Library::spacing_table_type spacingTable);

        via_type add_via(const via_name_type& viaName, const layer_name_to_via_geometry_type& layers);

        track_type add_track(const track_orientation_type& orientation, const Library::unit_type& start, const Library::scalar_type& num_tracks, const Library::unit_type& space, const layer_name_type& layer);

        template <typename Value>
        entity_system::Property<layer_type, Value> make_property_layer() const noexcept
        {
            return entity_system::Property<layer_type, Value>(m_layers);
        }

        template <typename Value>
        entity_system::Property<via_type, Value> make_property_via() const noexcept
        {
            return entity_system::Property<via_type, Value>(m_vias);
        }

        template <typename Value>
        entity_system::Property<track_type, Value> make_property_track() const noexcept
        {
            return entity_system::Property<track_type, Value>(m_tracks);
        }

        template <typename Value>
        entity_system::Aggregation<layer_type, Value> make_aggregation_layer(entity_system::EntitySystem<Value> & parts) const noexcept
        {
            return entity_system::Aggregation<layer_type, Value>(m_layers, parts);
        }

        entity_system::EntitySystem<layer_type>::NotifierType * notifier_layer() const noexcept;
        entity_system::EntitySystem<via_type>::NotifierType * notifier_via() const noexcept;
        entity_system::EntitySystem<track_type>::NotifierType * notifier_track() const noexcept;

    private:
        //Entities
        entity_system::EntitySystem<layer_type> m_layers{};
        entity_system::EntitySystem<via_type>   m_vias{};
        entity_system::EntitySystem<track_type> m_tracks{};

        //Properties
        entity_system::Property<layer_type, layer_name_type>      m_layer_names{m_layers};
        entity_system::Property<layer_type, layer_type_type>      m_layer_types{m_layers, layer_type_type::NA};
        entity_system::Property<layer_type, layer_direction_type> m_layer_directions{m_layers, layer_direction_type::NA};
        entity_system::Property<layer_type, unit_type>            m_layer_pitches{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_offsets{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_widths{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_min_widths{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_areas{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_spacing{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_end_of_line_spaces{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_end_of_line_widths{m_layers};
        entity_system::Property<layer_type, unit_type>            m_layer_end_of_line_withins{m_layers};
        entity_system::Property<layer_type, spacing_table_type>   m_layer_spacing_tables{m_layers};

        entity_system::Property<via_type, via_name_type>          m_via_names{m_vias};
        entity_system::Property<via_type, layer_name_to_via_geometry_type> m_via_layers_names_to_via_geometries{m_vias};

        entity_system::Property<track_type, track_orientation_type> m_track_orientations{m_tracks};
        entity_system::Property<track_type, unit_type>              m_track_starts{m_tracks};
        entity_system::Property<track_type, scalar_type>            m_number_of_tracks{m_tracks};
        entity_system::Property<track_type, unit_type>              m_track_spaces{m_tracks};
        entity_system::Property<track_type, layer_type>             m_layer_tracks{m_tracks};

        //Entity maps
        std::unordered_map<via_name_type, via_type>                 m_name_to_via{};
        std::unordered_map<layer_name_type, layer_type>             m_name_to_layer{};
    };
}

#endif // LIBRARY_H
