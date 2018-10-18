#ifndef OPHIDIAN_ROUTING_LIBRARY_H
#define OPHIDIAN_ROUTING_LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/util/Units.h>
#include <ophidian/util/LookupTable.h>
#include <unordered_map>
#include <ophidian/util/Range.h>

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
        template <class K, class V> using map_type  = std::unordered_map<K,V>;

        using scalar_type           = int;
        using unit_type             = util::database_unit_t;
        using table_strategy_type   = util::floor_strategy<unit_type, unit_type, unit_type>;
        using spacing_table_type    = util::LookupTable<unit_type, unit_type, unit_type, table_strategy_type>;
        using spacing_table_content_type = util::tableContents<unit_type, unit_type, unit_type>;

        using layer_type            = Layer;
        using layer_container_type  = std::vector<layer_type>;

        using via_type              = Via;
        using via_container_type    = std::vector<via_type>;
        using box_type              = geometry::Box<unit_type>;
        using via_layer_map_type    = map_type<std::string, box_type>;

        using track_type            = Track;
        using track_container_type  = std::vector<track_type>;

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
        layer_type find_layer_instance(const std::string& layerName) const;

        std::string& name(const layer_type& layer);
        const std::string& name(const layer_type& layer) const;
        LayerType type(const layer_type& layer) const;
        LayerDirection direction(const layer_type& layer) const;
        unit_type pitch(const layer_type& layer) const;
        unit_type offset(const layer_type& layer) const;
        unit_type width(const layer_type& layer) const;
        unit_type minWidth(const layer_type& layer) const;
        unit_type area(const layer_type& layer) const;
        unit_type spacing(const layer_type& layer) const;
        unit_type EOLspace(const layer_type& layer) const;
        unit_type EOLwidth(const layer_type& layer) const;
        unit_type EOLwithin(const layer_type& layer) const;
        const spacing_table_type& spacing_table(const layer_type& layer) const;

        via_type find_via_instance(const std::string& viaName) const;
        std::string& name(const via_type& via);
        const std::string& name(const via_type& via) const;
        const box_type &geometry(const via_type& via, const std::string &layer);

        const TrackOrientation orientation(const track_type& track) const;
        const unit_type start(const track_type& track) const;
        const scalar_type numTracks(const track_type& track) const;
        const unit_type space(const track_type& track) const;
        const layer_type layer(const track_type& track) const;

        // Iterators
        layer_container_type::const_iterator begin_layer() const noexcept;
        layer_container_type::const_iterator end_layer() const noexcept;

        via_container_type::const_iterator begin_via() const noexcept;
        via_container_type::const_iterator end_via() const noexcept;

        track_container_type::const_iterator begin_track() const noexcept;
        track_container_type::const_iterator end_track() const noexcept;

        ophidian::util::Range<track_container_type::const_iterator> range_track() const noexcept;

        // Capacity
        layer_container_type::size_type size_layer() const noexcept;
        via_container_type::size_type size_via() const noexcept;
        track_container_type::size_type size_track() const noexcept;



        // Modifiers
        layer_type add_layer_instance(const std::string &layerName, const LayerType &type, const LayerDirection &direction,
                                      const Library::unit_type& pitch, const Library::unit_type& offset, const Library::unit_type& width,
                                      const Library::unit_type& minWidth, const Library::unit_type& area, const Library::unit_type& spacing,
                                      const Library::unit_type& EOLspace, const Library::unit_type& EOLwidth, const Library::unit_type& EOLwithin,
                                      const Library::spacing_table_type spacingTable);

        via_type add_via_instance(const std::string &viaName, const via_layer_map_type & layers);

        track_type add_track_instance(const TrackOrientation &orientation, const Library::unit_type &start, const Library::scalar_type &numTracks, const Library::unit_type &space, const std::string &layer);

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

        template <typename Value>
        entity_system::Aggregation<layer_type, Value> makeAggregation(layer_type, entity_system::EntitySystem<Value> & parts) const
        {
            return entity_system::Aggregation<layer_type, Value>(mLayers, parts);
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
        std::unordered_map<std::string, via_type>               mName2Via{};
        entity_system::Property<via_type, via_layer_map_type>   mViaLayers{mVias};

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
