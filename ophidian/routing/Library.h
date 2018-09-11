#ifndef OPHIDIAN_ROUTING_LIBRARY_H
#define OPHIDIAN_ROUTING_LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Units.h>
#include <ophidian/util/LookupTable.h>

namespace ophidian
{
namespace routing
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


    class Library
    {
    public:
        using unit_type             = util::database_unit_t;
        using table_strategy_type   = util::floor_strategy<unit_type, unit_type, unit_type>;
        using spacing_table_type    = util::LookupTable<unit_type, unit_type, unit_type, table_strategy_type>;

        using box_type              = geometry::Box<unit_type>;
        using layer_pair_type       = std::pair<Layer ,box_type>;
        using layer_container_type  = std::vector<layer_pair_type>;

        // Constructors
        //! Construct Netlist
        Library() = default;

        //! coppy constructor
        Library(const Library &) = delete;
        Library & operator =(const Library &) = delete;

        //! Move Constructor
        Library(Library &&) = default;
        Library& operator=(Library &&) = default;

        // Element access

        // Iterators

        // Capacity

        // Modifiers

        template <typename Value>
        entity_system::Property<Layer, Value> makeProperty(Layer) const
        {
            return entity_system::Property<Layer, Value>(mLayers);
        }

        template <typename Value>
        entity_system::Property<Via, Value> makeProperty(Via) const
        {
            return entity_system::Property<Via, Value>(mVias);
        }

        entity_system::EntitySystem<Layer>::NotifierType * notifier(Layer) const;
        entity_system::EntitySystem<Via>::NotifierType * notifier(Via) const;

    private:
        entity_system::EntitySystem<Layer>  mLayers{};

        entity_system::Property<Layer, std::string>         mLayerName{mLayers};
        entity_system::Property<Layer, LayerType>           mLayerType{mLayers, LayerType::NA};
        entity_system::Property<Layer, LayerDirection>      mLayerDirection{mLayers, LayerDirection::NA};
        entity_system::Property<Layer, unit_type>           mLayerOffset{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerWidth{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerMinWidth{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerArea{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerSpacing{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerEOLspace{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerEOLwidth{mLayers};
        entity_system::Property<Layer, unit_type>           mLayerEOLwithin{mLayers};
        entity_system::Property<Layer, spacing_table_type>  mLayerSpacingTable{mLayers};

        entity_system::EntitySystem<Via>    mVias{};
        entity_system::Property<Via, std::string>           mViaName{mVias};
        entity_system::Property<Via, layer_container_type>  mViaLayers{mVias};
    };


} // namespace routing
} // namespace ophidian


#endif // LIBRARY_H
