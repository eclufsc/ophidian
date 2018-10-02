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

#include "Library.h"

namespace ophidian
{
namespace routing
{

Library::layer_type Library::find_layer_instance(const std::string &layerName)
{
    return mName2Layer.at(layerName);
}

std::string& Library::name(const Library::layer_type &layer)
{
    return mLayerName[layer];
}

const std::string& Library::name(const Library::layer_type &layer) const
{
    return mLayerName[layer];
}

Library::via_type Library::find_via_instance(const std::string &viaName)
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

const Library::box_type& Library::geometry(const Library::via_type &via, const std::string &layer)
{
    return  mViaLayers[via].find(layer)->second;
}

const TrackOrientation Library::orientation(const Library::track_type &track) const
{
    return mTrackOrientation[track];
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
    return mLayerOfTrack[track];
}

LayerType Library::type(const Library::layer_type &layer) const
{
    return mLayerType[layer];
}

LayerDirection Library::direction(const Library::layer_type &layer) const
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

const Library::spacing_table_type& Library::spacing_table(const Library::layer_type &layer) const
{
    return mLayerSpacingTable[layer];
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

Library::layer_type Library::add_layer_instance(
        const std::string &layerName,
        const LayerType &type,
        const LayerDirection &direction,
        const Library::unit_type& pitch,
        const Library::unit_type& offset,
        const Library::unit_type& width,
        const Library::unit_type& minWidth,
        const Library::unit_type& area,
        const Library::unit_type& spacing,
        const Library::unit_type& EOLspace,
        const Library::unit_type& EOLwidth,
        const Library::unit_type& EOLwithin,
        const Library::spacing_table_type spacingTable
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
        mLayerSpacingTable[layer] = spacingTable;

        return layer;
    }else{
        return mName2Layer[layerName];
    }
}

Library::via_type Library::add_via_instance(const std::string &viaName, const via_layer_map_type &layers)
{
    if(mName2Via.find(viaName) == mName2Via.end()){
        auto via = mVias.add();

        mViaName[via] = viaName;
        mName2Via[viaName] = via;
        mViaLayers[via] = layers;
    }else{
        return mName2Via[viaName];
    }
}

Library::track_type Library::add_track_instance(const TrackOrientation &orientation, const unit_type &start, const scalar_type &numTracks, const unit_type &space, const std::string &layer)
{
    auto track = mTracks.add();
    mTrackOrientation[track] = orientation;
    mTrackStart[track] = start;
    mNumberOfTracks[track] = numTracks;
    mTrackSpace[track] = space;
    mLayerOfTrack[track] = mName2Layer[layer];
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

} // namespace routing
} // namespace ophidian
