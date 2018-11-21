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

namespace ophidian::routing
{
    Library::layer_type Library::find_layer(const Library::layer_name_type &layerName) const
    {
        return m_name_to_layer.at(layerName);
    }

    Library::via_type Library::find_via(const Library::via_name_type &viaName) const
    {
        return m_name_to_via.at(viaName);
    }

    Library::layer_name_type& Library::name(const Library::layer_type &layer)
    {
        return m_layer_names[layer];
    }

    const Library::layer_name_type& Library::name(const Library::layer_type &layer) const
    {
        return m_layer_names[layer];
    }

    Library::layer_type_type& Library::type(const Library::layer_type &layer)
    {
        return m_layer_types[layer];
    }

    const Library::layer_type_type& Library::type(const Library::layer_type &layer) const
    {
        return m_layer_types[layer];
    }

    Library::layer_direction_type& Library::direction(const Library::layer_type &layer)
    {
        return m_layer_directions[layer];
    }

    const Library::layer_direction_type& Library::direction(const Library::layer_type &layer) const
    {
        return m_layer_directions[layer];
    }

    Library::unit_type& Library::pitch(const Library::layer_type &layer)
    {
        return m_layer_pitches[layer];
    }

    const Library::unit_type& Library::pitch(const Library::layer_type &layer) const
    {
        return m_layer_pitches[layer];
    }

    Library::unit_type& Library::offset(const Library::layer_type& layer)
    {
        return m_layer_offsets[layer];
    }

    const Library::unit_type& Library::offset(const Library::layer_type& layer) const
    {
        return m_layer_offsets[layer];
    }

    Library::unit_type& Library::width(const Library::layer_type& layer)
    {
        return m_layer_widths[layer];
    }

    const Library::unit_type& Library::width(const Library::layer_type& layer) const
    {
        return m_layer_widths[layer];
    }

    Library::unit_type& Library::min_width(const Library::layer_type& layer)
    {
        return m_layer_min_widths[layer];
    }

    const Library::unit_type& Library::min_width(const Library::layer_type& layer) const
    {
        return m_layer_min_widths[layer];
    }

    Library::unit_type& Library::area(const Library::layer_type& layer)
    {
        return m_layer_areas[layer];
    }

    const Library::unit_type& Library::area(const Library::layer_type& layer) const
    {
        return m_layer_areas[layer];
    }

    Library::unit_type& Library::spacing(const Library::layer_type& layer)
    {
        return m_layer_spacing[layer];
    }

    const Library::unit_type& Library::spacing(const Library::layer_type& layer) const
    {
        return m_layer_spacing[layer];
    }

    Library::unit_type& Library::EOLspace(const Library::layer_type& layer)
    {
        return m_layer_end_of_line_spaces[layer];
    }

    const Library::unit_type& Library::EOLspace(const Library::layer_type& layer) const
    {
        return m_layer_end_of_line_spaces[layer];
    }

    Library::unit_type& Library::EOLwidth(const Library::layer_type& layer)
    {
        return m_layer_end_of_line_widths[layer];
    }

    const Library::unit_type& Library::EOLwidth(const Library::layer_type& layer) const
    {
        return m_layer_end_of_line_widths[layer];
    }

    Library::unit_type& Library::EOLwithin(const Library::layer_type& layer)
    {
        return m_layer_end_of_line_withins[layer];
    }

    const Library::unit_type& Library::EOLwithin(const Library::layer_type& layer) const
    {
        return m_layer_end_of_line_withins[layer];
    }

    Library::spacing_table_type& Library::spacing_table(const Library::layer_type &layer)
    {
        return m_layer_spacing_tables[layer];
    }

    const Library::spacing_table_type& Library::spacing_table(const Library::layer_type &layer) const
    {
        return m_layer_spacing_tables[layer];
    }

    Library::via_name_type& Library::name(const Library::via_type &via)
    {
        return m_via_names[via];
    }

    const Library::via_name_type& Library::name(const Library::via_type &via) const
    {
        return m_via_names[via];
    }

    Library::via_geometry_type& Library::geometry(const Library::via_type &via, const Library::layer_name_type& layer)
    {
        return  m_via_layers_names_to_via_geometries[via].find(layer)->second;
    }

    const Library::via_geometry_type& Library::geometry(const Library::via_type &via, const Library::layer_name_type& layer) const
    {
        return  m_via_layers_names_to_via_geometries[via].find(layer)->second;
    }

    Library::track_orientation_type& Library::orientation(const Library::track_type &track)
    {
        return m_track_orientations[track];
    }

    const Library::track_orientation_type& Library::orientation(const Library::track_type &track) const
    {
        return m_track_orientations[track];
    }

    Library::unit_type& Library::start(const Library::track_type& track)
    {
        return m_track_starts[track];
    }

    const Library::unit_type& Library::start(const Library::track_type& track) const
    {
        return m_track_starts[track];
    }

    Library::scalar_type& Library::number_of_tracks(const Library::track_type& track)
    {
        return m_number_of_tracks[track];
    }

    const Library::scalar_type& Library::number_of_tracks(const Library::track_type& track) const
    {
        return m_number_of_tracks[track];
    }

    Library::unit_type& Library::space(const Library::track_type& track)
    {
        return m_track_spaces[track];
    }

    const Library::unit_type& Library::space(const Library::track_type& track) const
    {
        return m_track_spaces[track];
    }

    Library::layer_type& Library::layer(const Library::track_type& track)
    {
        return m_layer_tracks[track];
    }

    const Library::layer_type& Library::layer(const Library::track_type& track) const
    {
        return m_layer_tracks[track];
    }

    Library::layer_container_type::const_iterator Library::begin_layer() const noexcept
    {
        return m_layers.begin();
    }

    Library::layer_container_type::const_iterator Library::end_layer() const noexcept
    {
        return m_layers.end();
    }

    Library::via_container_type::const_iterator Library::begin_via() const noexcept
    {
        return m_vias.begin();
    }

    Library::via_container_type::const_iterator Library::end_via() const noexcept
    {
        return m_vias.end();
    }

    Library::track_container_type::const_iterator Library::begin_track() const noexcept
    {
        return m_tracks.begin();
    }

    Library::track_container_type::const_iterator Library::end_track() const noexcept
    {
        return m_tracks.end();
    }

    Library::layer_container_type::size_type Library::size_layer() const noexcept
    {
        return m_layers.size();
    }

    Library::via_container_type::size_type Library::size_via() const noexcept
    {
        return m_vias.size();
    }

    Library::track_container_type::size_type Library::size_track() const noexcept
    {
        return m_tracks.size();
    }

    Library::layer_type Library::add_layer(
        const Library::layer_name_type& layerName,
        const Library::layer_type_type& type,
        const Library::layer_direction_type& direction,
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
        if(m_name_to_layer.find(layerName) == m_name_to_layer.end()){
            auto layer = m_layers.add();

            m_layer_names[layer] = layerName;
            m_name_to_layer[layerName] = layer;
            m_layer_types[layer] = type;
            m_layer_directions[layer] = direction;
            m_layer_pitches[layer] = pitch;
            m_layer_offsets[layer] = offset;
            m_layer_widths[layer] = width;
            m_layer_min_widths[layer] = minWidth;
            m_layer_areas[layer] = area;
            m_layer_spacing[layer] = spacing;
            m_layer_end_of_line_spaces[layer] = EOLspace;
            m_layer_end_of_line_widths[layer] = EOLwidth;
            m_layer_end_of_line_withins[layer] = EOLwithin;
            m_layer_spacing_tables[layer] = spacingTable;

            return layer;
        }else{
            return m_name_to_layer[layerName];
        }
    }

    Library::via_type Library::add_via(const Library::via_name_type& viaName, const layer_name_to_via_geometry_type& layers)
    {
        if(m_name_to_via.find(viaName) == m_name_to_via.end()){
            auto via = m_vias.add();

            m_via_names[via] = viaName;
            m_name_to_via[viaName] = via;
            m_via_layers_names_to_via_geometries[via] = layers;
            return via;
        }else{
            return m_name_to_via[viaName];
        }
    }

    Library::track_type Library::add_track(
        const Library::track_orientation_type& orientation,
        const Library::unit_type& start,
        const Library::scalar_type& num_tracks,
        const Library::unit_type& space,
        const Library::layer_name_type& layer
    )
    {
        auto track = m_tracks.add();
        m_track_orientations[track] = orientation;
        m_track_starts[track] = start;
        m_number_of_tracks[track] = num_tracks;
        m_track_spaces[track] = space;
        m_layer_tracks[track] = m_name_to_layer[layer];
        return track;
    }

    entity_system::EntitySystem<Library::layer_type>::NotifierType * Library::notifier_layer() const noexcept
    {
        return m_layers.notifier();
    }

    entity_system::EntitySystem<Library::via_type>::NotifierType * Library::notifier_via() const noexcept
    {
        return m_vias.notifier();
    }

    entity_system::EntitySystem<Library::track_type>::NotifierType * Library::notifier_track() const noexcept
    {
        return m_tracks.notifier();
    }
}
