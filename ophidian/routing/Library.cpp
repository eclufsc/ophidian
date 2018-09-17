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
