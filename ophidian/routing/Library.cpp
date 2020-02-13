/*
 * Copyright 2017 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.
 */
#include <boost/lexical_cast.hpp>
#include "Library.h"

namespace ophidian
{
namespace routing
{

void Library::set_gcell_coordinates(Library::unit_container_type GCell_x_axis, Library::unit_container_type GCell_y_axis){
    mGCell_x_axis = GCell_x_axis;
    mGCell_y_axis = GCell_y_axis;
}

Library::box_type Library::gcell_box(const point_type & min_corner, const point_type & max_corner) const
{
    auto min_x_it = std::lower_bound(mGCell_x_axis.begin(), mGCell_x_axis.end(), min_corner.x(), [](const auto & lhs, const auto & rhs){return lhs <= rhs;});
    auto min_y_it = std::lower_bound(mGCell_y_axis.begin(), mGCell_y_axis.end(), min_corner.y(), [](const auto & lhs, const auto & rhs){return lhs <= rhs;});
    if(min_x_it != mGCell_x_axis.begin())
        min_x_it--;
    if(min_y_it != mGCell_y_axis.begin())
        min_y_it--;

    auto max_x_it = std::lower_bound(mGCell_x_axis.begin(), mGCell_x_axis.end(), max_corner.x());
    auto max_y_it = std::lower_bound(mGCell_y_axis.begin(), mGCell_y_axis.end(), max_corner.y());
    if(max_x_it == mGCell_x_axis.end())
        max_x_it--;
    if(max_y_it == mGCell_y_axis.end())
        max_y_it--;

    return box_type(point_type(unit_type(*min_x_it), unit_type(*min_y_it)),
                    point_type(unit_type(*max_x_it), unit_type(*max_y_it)));

}

const Library::layer_type Library::find_layer_instance(const std::string &layerName) const
{
    if (mName2Layer.find(layerName) != mName2Layer.end())
    {
        return mName2Layer.at(layerName);
    }
    else
    {
        return Library::layer_type{};
    }
}

std::string Library::name(const Library::layer_type &layer)
{
    return mLayerName[layer];
}

const std::string& Library::name(const Library::layer_type &layer) const
{
    return mLayerName[layer];
}

Library::via_type Library::find_via_instance(const std::string &viaName) const
{
    return mName2Via.at(viaName);
}

std::string& Library::name(const Library::via_type &via)
{
    return mViaName[via];
}

const std::string& Library::name(const Library::via_type &via) const
{
    return mViaName[via];
}

const Library::box_container_type Library::geometry(const Library::via_type &via, const Library::layer_type &layer)
{
    for(auto& geometry : mViaGeometries[via])
    {
        if(geometry.layer == layer){
            return geometry.boxes;
        }
    }
    // returns a reference to a temporary
    return  box_container_type();
}

const Library::box_container_type Library::geometry(const Library::via_type &via, const std::string &layer_name)
{
    auto layer = find_layer_instance(layer_name);
    for(auto& geometry : mViaGeometries[via])
    {
        if(geometry.layer == layer){
            return geometry.boxes;
        }
    }
    return  box_container_type();
}

const Direction Library::direction(const Library::track_type &track) const
{
    return mTrackDirection[track];
}

const Library::unit_type Library::start(const Library::track_type& track) const
{
    return mTrackStart[track];
}

const Library::scalar_type Library::numTracs(const Library::track_type& track) const
{
    return mNumberOfTracks[track];
}

const Library::unit_type Library::space(const Library::track_type& track) const
{
    return mTrackSpace[track];
}

const Library::layer_type Library::layer(const Library::track_type& track) const
{
    return mLayer2Tracks.whole(track);
}

LayerType Library::type(const Library::layer_type &layer) const
{
    return mLayerType[layer];
}

Direction Library::direction(const Library::layer_type &layer) const
{
    return mLayerDirection[layer];
}

Library::unit_type Library::pitch(const Library::layer_type &layer) const
{
    return mLayerPitch[layer];
}

Library::unit_type Library::offset(const Library::layer_type& layer) const
{
    return mLayerOffset[layer];
}

Library::unit_type Library::width(const Library::layer_type& layer) const
{
    return mLayerWidth[layer];
}

Library::unit_type Library::minWidth(const Library::layer_type& layer) const
{
    return mLayerMinWidth[layer];
}

Library::unit_type Library::area(const Library::layer_type& layer) const
{
    return mLayerArea[layer];
}

Library::unit_type Library::spacing(const Library::layer_type& layer) const
{
    return mLayerSpacing[layer];
}

Library::unit_type Library::EOLspace(const Library::layer_type& layer) const
{
    return mLayerEOLspace[layer];
}

Library::unit_type Library::EOLwidth(const Library::layer_type& layer) const
{
    return mLayerEOLwidth[layer];
}

Library::unit_type Library::EOLwithin(const Library::layer_type& layer) const
{
    return mLayerEOLwithin[layer];
}

Library::unit_type Library::EOLPspace(const Library::layer_type& layer) const
{
    return mLayerEOLPspace[layer];
}

Library::unit_type Library::EOLPwidth(const Library::layer_type& layer) const
{
    return mLayerEOLPwidth[layer];
}

Library::unit_type Library::EOLPwithin(const Library::layer_type& layer) const
{
    return mLayerEOLPwithin[layer];
}

Library::unit_type Library::EOLpar_space(const Library::layer_type& layer) const
{
    return mLayerEOLParSpace[layer];
}

Library::unit_type Library::EOLpar_within(const Library::layer_type& layer) const
{
    return mLayerEOLParWithin[layer];
}

Library::unit_type Library::ExceptEOLwidth(const Library::layer_type& layer) const
{
    return mLayerExceptEOLwidth[layer];
}

const Library::spacing_table_type& Library::spacing_table(const Library::layer_type &layer) const
{
    return mLayerSpacingTable[layer];
}

const Library::unit_type Library::adjacentSpacing(const Library::layer_type& layer) const
{
    return mLayerAdjSpacing[layer];
}

const Library::scalar_type Library::adjacentCuts(const Library::layer_type& layer) const
{
    return mLayerAdjCuts[layer];
}

const Library::unit_type Library::cutWithinLength(const Library::layer_type& layer) const
{
    return mLayerCutWithinLength[layer];
}

Library::layer_tracks_view_type Library::tracks(const Library::layer_type& layer) const
{
    return mLayer2Tracks.parts(layer);
}

const Library::track_type Library::prefTrack(const Library::layer_type& layer) const
{
    for(auto & track : mLayer2Tracks.parts(layer))
    {
        if(mLayerDirection[layer] == mTrackDirection[track])
        {
            return track;
        }
    }
    return Library::track_type();
}

const Library::track_type Library::nonPrefTrack(const Library::layer_type& layer) const
{

    for(auto & track : mLayer2Tracks.parts(layer))
    {
        if(mLayerDirection[layer] != mTrackDirection[track])
        {
            return track;
        }
    }
    return Library::track_type();
}


const Library::layer_type Library::upperLayer(const Library::layer_type& layer) const
{
    auto layerName = mLayerName[layer];
    std::string indexLayer = layerName.substr(5);
    int upperLayerIndex = boost::lexical_cast<int>(indexLayer) +1;
    std::string upperLayerName = "Metal" + boost::lexical_cast<std::string>(upperLayerIndex);
    return find_layer_instance(upperLayerName);
}

int Library::index(const Library::layer_type &layer) const
{
    auto layerName = mLayerName[layer];
    std::string indexLayer = layerName.substr(5);
    return boost::lexical_cast<int>(indexLayer);
}

const Library::layer_type Library::layer_from_index(int index) const
{
    std::string layerName = "Metal" + boost::lexical_cast<std::string>(index);
    return find_layer_instance(layerName);
}

const Library::layer_type Library::lowerLayer(const Library::layer_type& layer) const
{
    auto layerName = mLayerName[layer];
    std::string indexLayer = layerName.substr(5);
    int lowerLayerIndex = boost::lexical_cast<int>(indexLayer) -1;
    std::string lowerLayerName = "Metal" + boost::lexical_cast<std::string>(lowerLayerIndex);
    return find_layer_instance(lowerLayerName);
}

const Library::scalar_type Library::layerIndex(const Library::layer_type& layer) const
{
    auto layerName = mLayerName[layer];
    std::string indexLayerSegment = layerName.substr(5);
    int layerIndex = boost::lexical_cast<int>(indexLayerSegment) -1;
    std::string metalName = "Metal" + boost::lexical_cast<std::string>(layerIndex);
    return layerIndex;

}

const void Library::viaCandidates(Library::via_container_type& vias, const Library::layer_type& layer, const Library::layer_type& upperLayer) const
{
    for(auto & via : mLayer2Vias.parts(layer))
    {
        for(auto geometry : mViaGeometries[via])
        {
            if(geometry.layer == upperLayer)
            {
                vias.push_back(via);
                break;
            }
        }
    }
}

const  Library::layer_type  Library::viaLayerBelow(const  Library::layer_type& above) const
{
    auto layerName = mLayerName[above];
    std::string indexLayerSegment = layerName.substr(5);
    int layerIndex = boost::lexical_cast<int>(indexLayerSegment) -1;
    std::string viaName = "Via" + boost::lexical_cast<std::string>(layerIndex);
    return find_layer_instance(viaName);
}

bool Library::isFirstRightBelowSecond(const Library::layer_type& first, const Library::layer_type& second) const
{
    if (upperLayer(first) == second)
        return true;
    else
        return false;
}

Library::layer_type Library::highest_layer() const
{
    return this->mHighest_layer;
}

void Library::set_highest_layer(const layer_type& layer)
{
    this->mHighest_layer = layer;
}

Library::layer_container_type::const_iterator Library::begin_layer() const noexcept
{
    return mLayers.begin();
}

Library::layer_container_type::const_iterator Library::end_layer() const noexcept
{
    return mLayers.end();
}

Library::via_container_type::const_iterator Library::begin_via() const noexcept
{
    return mVias.begin();
}

Library::via_container_type::const_iterator Library::end_via() const noexcept
{
    return mVias.end();
}

Library::track_container_type::const_iterator Library::begin_track() const noexcept
{
    return mTracks.begin();
}

Library::track_container_type::const_iterator Library::end_track() const noexcept
{
    return mTracks.end();
}

Library::pad_container_type::const_iterator Library::begin_pad() const noexcept
{
    return mPads.begin();
}

Library::pad_container_type::const_iterator Library::end_pad() const noexcept
{
    return mPads.end();
}

Library::layer_container_type::size_type Library::size_layer() const noexcept
{
    return mLayers.size();
}

Library::via_container_type::size_type Library::size_via() const noexcept
{
    return mVias.size();
}

Library::track_container_type::size_type Library::size_track() const noexcept
{
    return mTracks.size();
}

Library::pad_container_type::size_type Library::size_pad() const noexcept
{
    return mPads.size();
}

std::string& Library::name(const Library::pad_type& pad)
{
    return mPadName[pad];
}

Library::pad_type Library::pad(const std::string padName)
{
    return map_name_to_pad[padName];
}

const Library::box_container_type Library::geometry(const Library::pad_type &pad, const Library::layer_type &layer) const
{
    for(auto& geometry : mPadGeometries[pad])
    {
        if(geometry.layer == layer){
            return geometry.boxes;
        }
    }
    // returns a reference to a temporary
    return  box_container_type();
}

const Library::box_container_type Library::geometry(const Library::pad_type &pad, const std::string &layer_name) const
{
    auto layer = find_layer_instance(layer_name);
    for(auto& geometry : mPadGeometries[pad])
    {
        if(geometry.layer == layer){
            return geometry.boxes;
        }
    }
    return  box_container_type();
}

Library::pad_geometries_container_type Library::geometries(const Library::pad_type& pad) const
{
    return mPadGeometries[pad];
}

Library::pad_geometries_container_type Library::geometries(const std::string padName)
{
    auto pad = map_name_to_pad[padName];
    return mPadGeometries[pad];
}

Library::point_type Library::position(const Library::pad_type& pad) const
{
    return mPadPosition[pad];
}

Library::orientation_type Library::orientation(const Library::pad_type& pad) const
{
    return mPadOrientation[pad];
}

std::map<std::string, Library::box_container_type> Library::box_in_layer(const Library::pad_type& pad) const
{
    std::map<std::string, box_container_type> map;
    for(auto geometry : mPadGeometries[pad])
    {
        auto layer_name = mLayerName[geometry.layer];
        for(auto box : geometry.boxes)
        {
            map[layer_name].push_back(box);
        }
    }
    return map;
}

Library::layer_type Library::add_layer_instance(
        const std::string &layerName,
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
)
{
    if(mName2Layer.find(layerName) == mName2Layer.end()){
        auto layer = mLayers.add();

        mLayerName[layer] = layerName;
        mName2Layer[layerName] = layer;
        mLayerType[layer] = type;
        mLayerDirection[layer] = direction;
        mLayerPitch[layer] = pitch;
        mLayerOffset[layer] = offset;
        mLayerWidth[layer] = width;
        mLayerMinWidth[layer] = minWidth;
        mLayerArea[layer] = area;
        mLayerSpacing[layer] = spacing;
        mLayerEOLspace[layer] = EOLspace;
        mLayerEOLwidth[layer] = EOLwidth;
        mLayerEOLwithin[layer] = EOLwithin;
        mLayerEOLPspace[layer] = EOLPspace;
        mLayerEOLPwidth[layer] = EOLPwidth;
        mLayerEOLPwithin[layer] = EOLPwithin;
        mLayerEOLParSpace[layer] = EOLParSpace;
        mLayerEOLParWithin[layer] = EOLParWithin;
        mLayerSpacingTable[layer] = spacingTable;
        mLayerAdjSpacing[layer] = adjSpacing;
        mLayerAdjCuts[layer] = adjCuts;
        mLayerCutWithinLength[layer] = cutWithinLength;
        mLayerExceptEOLwidth[layer] = ExceptEOLwidth;
        return layer;
    }else{
        return mName2Layer[layerName];
    }
}

Library::via_type Library::add_via_instance(const std::string &viaName, const Library::via_geometries_container_type &layers)
{
    if(mName2Via.find(viaName) == mName2Via.end()){
        auto via = mVias.add();

        mViaName[via] = viaName;
        mName2Via[viaName] = via;
        mViaGeometries[via] = layers;

        for(auto geometry_layer : layers)
        {
            mLayer2Vias.addAssociation(geometry_layer.layer, via);
        }

        return via;
    }else{
        return mName2Via[viaName];
    }
}

Library::track_type Library::add_track_instance(const Direction &direction, const unit_type &start, const scalar_type &numTracks, const unit_type &space, const std::string &layer)
{
    auto track = mTracks.add();
    mTrackDirection[track] = direction;
    mTrackStart[track] = start;
    mNumberOfTracks[track] = numTracks;
    mTrackSpace[track] = space;
    // mLayerOfTrack[track] = mName2Layer[layer];

    mLayer2Tracks.addAssociation(mName2Layer[layer], track);
    return track;
}

Library::pad_type Library::add_pad_instance(
    const std::string padName,
    const Library::point_type position,
    const Library::orientation_type orientation,
    const Library::pad_geometries_container_type & geometries
)
{
    auto pad = mPads.add();
    mPadName[pad] = padName;
    mPadPosition[pad] = position;
    mPadOrientation[pad] = orientation;
    mPadGeometries[pad] = geometries;
    map_name_to_pad.insert(std::make_pair(padName, pad));
    return pad;
}

entity_system::EntitySystem<Library::layer_type>::NotifierType * Library::notifier(Library::layer_type) const {
    return mLayers.notifier();
}

entity_system::EntitySystem<Library::via_type>::NotifierType * Library::notifier(Library::via_type) const {
    return mVias.notifier();
}

entity_system::EntitySystem<Library::track_type>::NotifierType * Library::notifier(Library::track_type) const {
    return mTracks.notifier();
}

entity_system::EntitySystem<Library::pad_type>::NotifierType * Library::notifier(Library::pad_type) const {
    return mPads.notifier();
}

} // namespace routing
} // namespace ophidian
