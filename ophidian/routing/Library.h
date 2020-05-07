#ifndef OPHIDIAN_ROUTING_LIBRARY_H
#define OPHIDIAN_ROUTING_LIBRARY_H

#include <ophidian/circuit/StandardCells.h>
#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/entity_system/Composition.h>
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

    enum class Direction {
        HORIZONTAL, VERTICAL, NA
    };

    enum class Orientation : int {
        N, S, W, E, FN, FS, FW, FE
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

    class Pad :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class RoutingBlockage :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    struct geometry_in_layer_type
    {
    public:
        using unit_type             = util::database_unit_t;
        using box_type              = geometry::Box<unit_type>;
        using box_container_type    = std::vector<box_type>;

        Layer layer;
        box_container_type boxes;
    };

    class Library
    {
    public:
        template <class K, class V> using map_type  = std::unordered_map<K,V>;

        using scalar_type                      = int;
        using unit_type                        = util::database_unit_t;
        using table_strategy_type              = util::FloorStrategy<unit_type, unit_type, unit_type>;
        using spacing_table_type               = util::LookupTable<unit_type, unit_type, unit_type, table_strategy_type>;
        using spacing_table_content_type       = util::TableContents<unit_type, unit_type, unit_type>;

        using layer_type                       = Layer;
        using layer_container_type             = std::vector<layer_type>;

        using via_type                         = Via;
        using via_container_type               = std::vector<via_type>;
        using box_type                         = geometry::Box<unit_type>;
        using box_container_type               = std::vector<box_type>;
        using via_geometries_container_type    = std::vector<geometry_in_layer_type>;
        using point_type                       = ophidian::util::LocationDbu;

        using track_type                       = Track;
        using track_container_type             = std::vector<track_type>;

        using blockage_type                    = RoutingBlockage;
        using blockage_container_type          = std::vector<blockage_type>;
        using std_cell_type                    = ophidian::circuit::Cell;

        using pad_type                         = Pad;
        using pad_container_type               = std::vector<pad_type>;
        using pad_geometries_container_type    = std::vector<geometry_in_layer_type>;
        using orientation_type                 = Orientation;

        using layer_tracks_view_type           = entity_system::Association<layer_type, track_type>::Parts;
        using blockages_view_type              = entity_system::Association<std_cell_type, blockage_type>::Parts;

        // Constructors
        //! Construct Netlist
        Library() = delete;

        //! coppy constructor
        Library(const Library &) = delete;
        Library & operator =(const Library &) = delete;

        //! Move Constructor
        Library(Library &&) = default;
        Library& operator=(Library &&) = default;

        Library(ophidian::circuit::StandardCells& std_cells);

        // Element access
        const layer_type find_layer_instance(const std::string& layerName) const;

        std::string name(const layer_type& layer);
        const std::string& name(const layer_type& layer) const;
        int index(const layer_type& layer) const;
        const layer_type layer_from_index(int index) const;
        LayerType type(const layer_type& layer) const;
        Direction direction(const layer_type& layer) const;
        unit_type pitch(const layer_type& layer) const;
        unit_type offset(const layer_type& layer) const;
        unit_type width(const layer_type& layer) const;
        unit_type minWidth(const layer_type& layer) const;
        unit_type area(const layer_type& layer) const;
        unit_type spacing(const layer_type& layer) const;
        unit_type EOLspace(const layer_type& layer) const;
        unit_type EOLwidth(const layer_type& layer) const;
        unit_type EOLwithin(const layer_type& layer) const;
        unit_type EOLPspace(const layer_type& layer) const;
        unit_type EOLPwidth(const layer_type& layer) const;
        unit_type EOLPwithin(const layer_type& layer) const;
        unit_type EOLpar_space(const layer_type& layer) const;
        unit_type EOLpar_within(const layer_type& layer) const;
        unit_type ExceptEOLwidth(const layer_type& layer) const;
        const spacing_table_type& spacing_table(const layer_type& layer) const;
        layer_tracks_view_type tracks(const layer_type& layer) const;
        const unit_type adjacentSpacing(const layer_type& layer) const;
        const scalar_type adjacentCuts(const layer_type& layer) const;
        const unit_type cutWithinLength(const layer_type& layer) const;

        const track_type prefTrack(const layer_type& layer) const;
        const track_type nonPrefTrack(const layer_type& layer) const;
        const layer_type upperLayer(const layer_type& layer) const;
        const layer_type lowerLayer(const layer_type& layer) const;
        const scalar_type layerIndex(const layer_type& layer) const;//Be careful, use only for sorting layers.
        const void viaCandidates(via_container_type& vias, const layer_type& layer, const layer_type& upperLayer) const;
        //! returns the via layer (cut) below the above layer
        const layer_type viaLayerBelow(const layer_type& above) const;
        //! returns true if the second layer is above the first
        bool isFirstRightBelowSecond(const layer_type& first, const layer_type& second) const;

        via_type find_via_instance(const std::string& viaName) const;
        std::string& name(const via_type& via);
        const std::string& name(const via_type& via) const;
        const box_container_type geometry(const via_type& via, const layer_type &layer);
        const box_container_type geometry(const via_type& via, const std::string &layer_name);

        const Direction direction(const track_type& track) const;
        const unit_type start(const track_type& track) const;
        const scalar_type numTracs(const track_type& track) const;
        const unit_type space(const track_type& track) const;
        const layer_type layer(const track_type& track) const;

        std::string& name(const pad_type& pad);
        pad_type pad(const std::string padName);
        const box_container_type geometry(const pad_type& pad, const layer_type &layer) const;
        const box_container_type geometry(const pad_type& pad, const std::string &layer_name) const;
        pad_geometries_container_type geometries(const pad_type& pad) const;
        pad_geometries_container_type geometries(const std::string padName);
        point_type position(const pad_type& pad) const;
        orientation_type orientation (const pad_type& pad) const;
        std::map<std::string, box_container_type> box_in_layer(const pad_type& pad) const;

        layer_type lowest_layer() const;
        scalar_type lowest_layer_index() const;
        // void set_lowest_layer(const layer_type& layer);
        
        layer_type highest_layer() const;
        scalar_type highest_layer_index() const;
        // void set_highest_layer(const layer_type& layer);

        std::string name(const blockage_type& blkg) const;
        std_cell_type std_cell(const blockage_type& blkg) const;
        layer_type layer(const blockage_type& blkg) const;
        scalar_type demand(const blockage_type& blkg) const;
        blockage_type find_blockage(const std::string& blockage_name) const;
        blockages_view_type blockages(const std_cell_type& std_cell) const;

        // Iterators
        layer_container_type::const_iterator begin_layer() const noexcept;
        layer_container_type::const_iterator end_layer() const noexcept;

        via_container_type::const_iterator begin_via() const noexcept;
        via_container_type::const_iterator end_via() const noexcept;

        track_container_type::const_iterator begin_track() const noexcept;
        track_container_type::const_iterator end_track() const noexcept;

        pad_container_type::const_iterator begin_pad() const noexcept;
        pad_container_type::const_iterator end_pad() const noexcept;

        blockage_container_type::const_iterator begin_blockages() const noexcept;
        blockage_container_type::const_iterator end_blockages() const noexcept;

        // Capacity
        layer_container_type::size_type size_layer() const noexcept;
        via_container_type::size_type size_via() const noexcept;
        track_container_type::size_type size_track() const noexcept;
        pad_container_type::size_type size_pad() const noexcept;
        blockage_container_type::size_type size_blockage() const noexcept;



        // Modifiers
        layer_type add_layer_instance(
        const std::string &layerName,
        const Library::scalar_type index,
        const LayerType &type,
        const Direction &direction,
        const Library::unit_type& pitch,
        const Library::unit_type& offset,
        const Library::unit_type& width,
        const Library::unit_type& minWidth,
        const Library::unit_type& area,
        const Library::unit_type& spacing,
        const Library::unit_type& EOLspace,
        const Library::unit_type& EOLwidth,
        const Library::unit_type& EOLwithin,
        const Library::unit_type& EOLPspace,
        const Library::unit_type& EOLPwidth,
        const Library::unit_type& EOLPwithin,
        const Library::unit_type& EOLParSpace,
        const Library::unit_type& EOLParWithin,
        const Library::spacing_table_type spacingTable,
        const Library::unit_type& adjSpacing,
        const Library::scalar_type adjCuts,
        const Library::unit_type cutWithinLength,
        const Library::unit_type& ExceptEOLwidth
        );

        blockage_type add_blockage(const std::string blockage_name, const std_cell_type &std_cell, const layer_type layer, scalar_type demand);

        via_type add_via_instance(const std::string &viaName, const via_geometries_container_type & layers);

        track_type add_track_instance(const Direction &direction, const Library::unit_type &start, const Library::scalar_type &numTracks, const Library::unit_type &space, const std::string &layer);

        pad_type add_pad_instance(const std::string padName, const point_type position, const orientation_type orientation, const pad_geometries_container_type & geometries );

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
        entity_system::Property<pad_type, Value> makeProperty(pad_type) const
        {
            return entity_system::Property<pad_type, Value>(mPads);
        }

        template <typename Value>
        entity_system::Aggregation<layer_type, Value> makeAggregation(layer_type, entity_system::EntitySystem<Value> & parts) const
        {
            return entity_system::Aggregation<layer_type, Value>(mLayers, parts);
        }

        entity_system::EntitySystem<layer_type>::NotifierType * notifier(layer_type) const;
        entity_system::EntitySystem<via_type>::NotifierType * notifier(via_type) const;
        entity_system::EntitySystem<track_type>::NotifierType * notifier(track_type) const;
        entity_system::EntitySystem<pad_type>::NotifierType * notifier(pad_type) const;
        entity_system::EntitySystem<blockage_type>::NotifierType * notifier(blockage_type) const;

    private:
        layer_type                                              mLowest_layer;
        scalar_type                                             mLowest_layer_index;
        layer_type                                              mHighest_layer;
        scalar_type                                             mHighest_layer_index;

        entity_system::EntitySystem<layer_type>                 mLayers{};

        entity_system::EntitySystem<blockage_type>              mBlockages{};
        entity_system::Property<blockage_type, std::string>     mBlockageNames{mBlockages};
        entity_system::Property<blockage_type, layer_type>      mBlockageLayer{mBlockages};
        entity_system::Property<blockage_type, scalar_type>     mBlockageDemand{mBlockages};
        std::unordered_map<std::string, blockage_type>          mName2Blockage{};
        entity_system::Composition<std_cell_type, blockage_type>mCell2Blockages;

        entity_system::Property<layer_type, std::string>        mLayerName{mLayers};
        std::unordered_map<std::string, layer_type>             mName2Layer{};
        entity_system::Property<layer_type, scalar_type>        mLayerIndexes{mLayers};
        entity_system::Property<layer_type, LayerType>          mLayerType{mLayers, LayerType::NA};
        entity_system::Property<layer_type, Direction>          mLayerDirection{mLayers, Direction::NA};
        entity_system::Property<layer_type, unit_type>          mLayerPitch{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerOffset{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerWidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerMinWidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerArea{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerSpacing{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLspace{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLwidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLwithin{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLPspace{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLPwidth{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLPwithin{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLParSpace{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerEOLParWithin{mLayers};
        entity_system::Property<layer_type, spacing_table_type> mLayerSpacingTable{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerAdjSpacing{mLayers};
        entity_system::Property<layer_type, scalar_type>        mLayerAdjCuts{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerCutWithinLength{mLayers};
        entity_system::Property<layer_type, unit_type>          mLayerExceptEOLwidth{mLayers};
        

        entity_system::EntitySystem<via_type>    mVias{};
        entity_system::Property<via_type, std::string>          mViaName{mVias};
        std::unordered_map<std::string, via_type>               mName2Via{};
        entity_system::Property<via_type, via_geometries_container_type>   mViaGeometries{mVias};

        entity_system::EntitySystem<track_type>    mTracks{};
        entity_system::Property<track_type, Direction>          mTrackDirection{mTracks};
        entity_system::Property<track_type, unit_type>          mTrackStart{mTracks};
        entity_system::Property<track_type, scalar_type>        mNumberOfTracks{mTracks};
        entity_system::Property<track_type, unit_type>          mTrackSpace{mTracks};

        entity_system::EntitySystem<pad_type>    mPads{};
        entity_system::Property<pad_type, std::string>          mPadName{mPads};
        entity_system::Property<pad_type, orientation_type>     mPadOrientation{mPads};
        entity_system::Property<pad_type, point_type>           mPadPosition{mPads};
        entity_system::Property<pad_type, pad_geometries_container_type>   mPadGeometries{mPads};
        std::unordered_map<std::string, pad_type> map_name_to_pad;

        entity_system::Aggregation<layer_type, track_type>      mLayer2Tracks{mLayers, mTracks};
        entity_system::Aggregation<layer_type, via_type>        mLayer2Vias{mLayers, mVias};
    };
}

#endif // LIBRARY_H
