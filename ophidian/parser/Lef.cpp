/*
 * Copyright 2016 Ophidian
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

#include <memory>
#include <string>

#include <lefrReader.hpp>

#include "Lef.h"
#include "ParserException.h"

namespace ophidian::parser
{
    Lef::Lef(const std::string& lef_file):
        m_sites{},
        m_layers{},
        m_macros{},
        m_micrometer_to_dbu_ratio{0}
    {
        read_file(lef_file);
    }

    Lef::Lef(const std::vector<std::string>& lef_files):
        m_sites{},
        m_layers{},
        m_macros{},
        m_micrometer_to_dbu_ratio{0}
    {
        for(const auto& lef_file : lef_files)
        {
            read_file(lef_file);
        }
    }

    void Lef::read_file(const std::string& lef_file)
    {
        lefrInit();
        lefrSetUnitsCbk(
            [](lefrCallbackType_e, lefiUnits * units, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);
                that->m_micrometer_to_dbu_ratio = Lef::scalar_type{units->databaseNumber()};
                return 0;
            }
        );

        lefrSetSiteCbk(
            [](lefrCallbackType_e, lefiSite * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                that->m_sites.emplace_back(
                    l->name(),
                    [&](){ return (l->hasClass() ? l->siteClass() : ""); }(),
                    Lef::site_type::micrometer_type{l->sizeX()},
                    Lef::site_type::micrometer_type{l->sizeY()},
                    Lef::site_type::symmetry_type{l->hasXSymmetry(), l->hasYSymmetry(), l->has90Symmetry()}
                );

                return 0;
            }
        );

        lefrSetLayerCbk(
            [](lefrCallbackType_e, lefiLayer * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                that->m_layers.emplace_back(
                    l->name(),
                    [&](){
                        if(l->hasType()){
                            auto type_name = std::string{l->type()};
                            if (type_name == "MASTERSLICE" ||
                                   type_name == "masterslice"){
                                return Lef::layer_type::type_type::MASTERSLICE;
                            }
                            else if(type_name == "CUT" ||
                                    type_name == "cut"){
                                return Lef::layer_type::type_type::CUT;
                            }
                            else if(type_name == "ROUTING" ||
                                    type_name == "routing"){
                                return Lef::layer_type::type_type::ROUTING;
                            }
                        }
                        return Lef::layer_type::type_type::NOT_ASSIGNED;
                    }(),
                    [&](){
                        if(l->hasDirection()){
                            auto direction_name = std::string(l->direction());
                            if(direction_name == "HORIZONTAL" ||
                               direction_name == "horizontal")
                            {
                                return Lef::layer_type::direction_type::HORIZONTAL;
                            }
                            else if(direction_name == "VERTICAL" ||
                                    direction_name == "vertical")
                            {
                                return Lef::layer_type::direction_type::VERTICAL;
                            }
                        }
                        return Lef::layer_type::direction_type::NOT_ASSIGNED;
                    }(),
                    Lef::layer_type::micrometer_type{l->pitch()},
                    Lef::layer_type::micrometer_type{l->offset()},
                    Lef::layer_type::micrometer_type{l->width()},

                    Lef::layer_type::micrometer_type{l->minwidth()},
                    Lef::layer_type::micrometer_type{l->area()},
                    [&](){
                        int i = -1;
                        for (int index = 0; index < l->numSpacing(); ++index) {
                            if(!l->hasSpacingEndOfLine(index)){
                                i = index;
                                break;
                            }
                        }
                        if(i >= 0){
                            return Lef::layer_type::micrometer_type{l->spacing(i)};
                        }else{
                            return Lef::layer_type::micrometer_type{0.0};
                        }
                    }(),
                    [&](){
                        int i = -1;
                        for (int index = 0; index < l->numSpacing(); ++index) {
                            if(l->hasSpacingEndOfLine(index)){
                                i = index;
                                break;
                            }
                        }
                        if(i >= 0){
                            return Lef::layer_type::end_of_line_type{l->spacing(i), l->spacingEolWidth(i), l->spacingEolWithin(i)};
                        }else{
                            return Lef::layer_type::end_of_line_type{0.0, 0.0, 0.0};
                        }
                    }(),
                    [&](){
                        if(l->numSpacingTable() == 0){
                            return Lef::layer_type::parallel_run_length_type{};
                        }
                        if(l->spacingTable(0)->isParallel())
                        {
                            auto lengths = Lef::layer_type::parallel_run_length_type::length_container_type{};
                            auto widths = Lef::layer_type::parallel_run_length_type::width_container_type{};
                            auto width_length_to_spacing = Lef::layer_type::parallel_run_length_type::spacing_container_type{};

                            auto parallel = l->spacingTable(0)->parallel();

                            for (int i = 0; i < parallel->numLength(); ++i)
                            {
                                lengths.emplace_back(micrometer_type{parallel->length(i)});

                                for (int j = 0; j < parallel->numWidth(); ++j) {
                                   widths.emplace_back(micrometer_type{parallel->width(j)});
                                   width_length_to_spacing[{widths.back(), lengths.back()}] = Lef::layer_type::parallel_run_length_type::spacing_type{micrometer_type{parallel->widthSpacing(i, j)}};
                                }
                            }
                            return Lef::layer_type::parallel_run_length_type{
                                std::move(widths),
                                std::move(lengths),
                                std::move(width_length_to_spacing)
                            };
                        }else{
                            return Lef::layer_type::parallel_run_length_type{};
                        }
                    }()
                );

                return 0;
            }
        );

        lefrSetMacroBeginCbk(
            [](lefrCallbackType_e, const char * macro_name, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                using micron_t       = Lef::macro_type::micrometer_type;
                using micron_point_t = Lef::macro_type::micrometer_point_type;

                that->m_macros.emplace_back(
                    macro_name,
                    "",
                    Lef::macro_type::foreign_type{"", micron_point_t{micron_t{0}, micron_t{0}}},
                    Lef::macro_type::micrometer_point_type{micron_t{0}, micron_t{0}},
                    Lef::macro_type::micrometer_point_type{micron_t{0}, micron_t{0}},
                    "",
                    Lef::macro_type::pin_container_type{},
                    Lef::macro_type::obstruction_map_type{}
                );

                return 0;
            }
        );

        lefrSetMacroCbk(
            [](lefrCallbackType_e, lefiMacro * l, lefiUserData ud) -> int {
                auto& m = static_cast<Lef *>(ud)->m_macros.back();
                m.name() = l->name();
                m.class_name() = (l->hasClass() ? l->macroClass() : "");
                if(l->hasForeign()) {
                    m.foreign().name = l->foreignName();
                    m.foreign().offset = Lef::macro_type::micrometer_point_type{
                        Lef::macro_type::micrometer_type{l->foreignX()},
                        Lef::macro_type::micrometer_type{l->foreignY()}
                    };
                }
                m.origin() = Lef::macro_type::micrometer_point_type{
                    Lef::macro_type::micrometer_type{l->originX()},
                    Lef::macro_type::micrometer_type{l->originY()}
                };
                m.size() = Lef::macro_type::micrometer_point_type{
                    Lef::macro_type::micrometer_type{l->sizeX()},
                    Lef::macro_type::micrometer_type{l->sizeY()}
                };
                m.site() = (l->hasSiteName() ? l->siteName() : "");

                return 0;
            }
        );

        lefrSetPinCbk(
            [](lefrCallbackType_e, lefiPin * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto ports = Lef::macro_type::pin_type::port_map_type{};

                for(int i = 0; i < l->numPorts(); ++i)
                {
                    auto layer_name = Lef::macro_type::pin_type::port_map_key_type{""};
                    for(int j = 0; j < l->port(i)->numItems(); ++j)
                    {
                        switch(l->port(i)->itemType(j))
                        {
                        case lefiGeomLayerE:
                            layer_name = l->port(i)->getLayer(j);
                            break;

                        case lefiGeomRectE:
                            ports[layer_name].emplace_back(
                                Lef::macro_type::pin_type::micrometer_point_type{
                                    Lef::macro_type::pin_type::micrometer_type{l->port(i)->getRect(j)->xl},
                                    Lef::macro_type::pin_type::micrometer_type{l->port(i)->getRect(j)->yl}
                                }, Lef::macro_type::pin_type::micrometer_point_type{
                                    Lef::macro_type::pin_type::micrometer_type{l->port(i)->getRect(j)->xh},
                                    Lef::macro_type::pin_type::micrometer_type{l->port(i)->getRect(j)->yh}
                                }
                            );
                            break;
                        }
                    }
                }

                that->m_macros.back().pins().emplace_back(
                    l->name(),
                    [&](){
                        if(l->hasDirection()) {
                            if(std::string(l->direction()) == "INPUT") {
                                return Lef::macro_type::pin_type::direction_type::INPUT;
                            }
                            else if(std::string(l->direction()) == "OUTPUT") {
                                return Lef::macro_type::pin_type::direction_type::OUTPUT;
                            }
                            else if(std::string(l->direction()) == "INOUT") {
                                return Lef::macro_type::pin_type::direction_type::INOUT;
                            }
                        }
                        return Lef::macro_type::pin_type::direction_type::NA;
                    }(),
                    std::move(ports)
                );
                return 0;
            }
        );

        lefrSetObstructionCbk(
            [](lefrCallbackType_e, lefiObstruction * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto geometries = l->geometries();

                auto last_layer = Lef::macro_type::obstruction_map_key_type{""};
                for(int i = 0; i < geometries->numItems(); ++i)
                {
                    switch(geometries->itemType(i))
                    {
                    case lefiGeomLayerE:
                        last_layer = geometries->getLayer(i);
                        break;

                    case lefiGeomRectE:
                        auto geom_rect = geometries->getRect(i);
                        that->m_macros.back().obstructions()[last_layer].emplace_back(
                            Lef::macro_type::micrometer_point_type{
                                Lef::macro_type::micrometer_type{geom_rect->xl},
                                Lef::macro_type::micrometer_type{geom_rect->yl}
                            },
                            Lef::macro_type::micrometer_point_type{
                                Lef::macro_type::micrometer_type{geom_rect->xh},
                                Lef::macro_type::micrometer_type{geom_rect->yh}
                            }
                        );
                        break;
                    }
                }

                return 0;
            }
        );

        lefrSetMacroEndCbk(
            [](lefrCallbackType_e, const char * string, lefiUserData) -> int {
                return 0;
            }
        );

        lefrSetViaCbk(
            [](lefrCallbackType_e, lefiVia* l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);
                auto via = Lef::via_type{l->name()};

                for (int i = 0; i < l->numLayers(); ++i) {
//                    auto layerName = l->layerName(i);
//                    auto numRects = l->numRects(i);
//                    auto xl = l->xl(i, 0);
//                    auto yl = l->yl(i, 0);
//                    auto xh = l->xh(i, 0);
//                    auto yh = l->yh(i, 0);
                    Lef::via_type::layer_container_type boxes;
                    boxes.reserve(l->numRects(i));
                    for (int j = 0; j < l->numRects(i); ++j) {
                        using micrometer = Lef::via_type::micrometer_type;
                        using micrometerPoint = Lef::via_type::micrometer_point_type;
                        auto pl = micrometerPoint{micrometer{l->xl(i, j)}, micrometer{l->yl(i, j)}};
                        auto ph = micrometerPoint{micrometer{l->xh(i, j)}, micrometer{l->yh(i, j)}};
                        auto box = Lef::via_type::micrometer_box_type{pl, ph};
                        boxes.push_back(box);
                    }
                    via.addLayer(std::string(l->layerName(i)), boxes);
                }
                that->m_vias.emplace_back(via);
                return 0;
            }
        );

        auto fp = std::unique_ptr<FILE, decltype( & std::fclose)>(
            std::fopen(lef_file.c_str(), "r"),
            &std::fclose);

        if(!fp) {
            throw exceptions::InexistentFile();
        }

        lefrRead(fp.get(), lef_file.c_str(), this);
        lefrClear();
    }

    const Lef::site_container_type& Lef::sites() const noexcept
    {
        return m_sites;
    }

    const Lef::layer_container_type& Lef::layers() const noexcept
    {
        return m_layers;
    }

    const Lef::macro_container_type& Lef::macros() const noexcept
    {
        return m_macros;
    }

    const Lef::scalar_type& Lef::micrometer_to_dbu_ratio() const noexcept
    {
        return m_micrometer_to_dbu_ratio;
    }

    const Lef::via_container_type& Lef::vias() const noexcept
    {
        return m_vias;
    }
}
