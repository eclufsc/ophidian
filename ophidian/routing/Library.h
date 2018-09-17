#ifndef OPHIDIAN_ROUTING_LIBRARY_H
#define OPHIDIAN_ROUTING_LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Units.h>
#include <ophidian/util/LookupTable.h>
#include <unordered_map>

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
        using table_strategy_type   = util::floor_strategy<unit_type, unit_type, unit_type>;
        using spacing_table_type    = util::LookupTable<unit_type, unit_type, unit_type, table_strategy_type>;
        using spacing_table_content_type = util::tableContents<unit_type, unit_type, unit_type>;

        using layer_type            = Layer;
        using box_type              = geometry::Box<unit_type>;
        using layer_pair_type       = std::pair<layer_type ,box_type>;
        using layer_container_type  = std::vector<layer_pair_type>;

        using via_type              = Via;
        using track_type            = Track;

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
        layer_type find_layer_instance(const std::string& layerName);
        // Iterators

        // Capacity

        // Modifiers
        layer_type add_layer_instance(const std::string &name, const LayerType &type, const LayerDirection &direction,
                                      const Library::unit_type& pitch, const Library::unit_type& offset, const Library::unit_type& width,
                                      const Library::unit_type& minWidth, const Library::unit_type& area, const Library::unit_type& spacing,
                                      const Library::unit_type& EOLspace, const Library::unit_type& EOLwidth, const Library::unit_type& EOLwithin,
                                      const Library::spacing_table_type spacingTable);

        template <typename Value>
        entity_system::Property<layer_type, Value> makeProperty(layer_type) const
        {
            return entity_system::Property<layer_type, Value>(mLayers);
        }

        template <typename Value>
        entity_system::Property<via_type, Value> makeProperty(via_type) const
        {
            return entity_system::Property<via_type, Value>(mVias);
        }

        template <typename Value>
        entity_system::Property<track_type, Value> makeProperty(track_type) const
        {
            return entity_system::Property<track_type, Value>(mTracks);
        }

        entity_system::EntitySystem<layer_type>::NotifierType * notifier(layer_type) const;
        entity_system::EntitySystem<via_type>::NotifierType * notifier(via_type) const;
        entity_system::EntitySystem<track_type>::NotifierType * notifier(track_type) const;

    private:
        entity_system::EntitySystem<layer_type>  mLayers{};
        entity_system::Property<layer_type, std::string>        mLayerName{mLayers};
        std::unordered_map<std::string, layer_type>             mName2Layer{};
        entity_system::Property<layer_type, LayerType>          mLayerType{mLayers, LayerType::NA};
        entity_system::Property<layer_type, LayerDirection>     mLayerDirection{mLayers, LayerDirection::NA};
        entity_system::Property<layer_type, unit_type>          mLayerPitch{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerOffset{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerWidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerMinWidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerArea{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerSpacing{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLspace{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLwidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLwithin{mLayers};
        entity_system::Property<layer_type, spacing_table_type> mLayerSpacingTable{mLayers};

        entity_system::EntitySystem<via_type>    mVias{};
        entity_system::Property<via_type, std::string>          mViaName{mVias};
        entity_system::Property<via_type, layer_container_type> mViaLayers{mVias};

        entity_system::EntitySystem<track_type>    mTracks{};
        entity_system::Property<track_type, TrackOrientation>   mTrackOrientation{mTracks};
        entity_system::Property<track_type, unit_type>          mTrackStart{mTracks};
        entity_system::Property<track_type, scalar_type>        mNumberOfTracks{mTracks};
        entity_system::Property<track_type, unit_type>          mTrackSpace{mTracks};
        entity_system::Property<track_type, layer_type>         mLayerOfTrack{mTracks};
    };


} // namespace routing
} // namespace ophidian


#endif // LIBRARY_H
