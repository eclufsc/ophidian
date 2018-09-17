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

namespace ophidian
{
namespace parser
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
                        return Lef::layer_type::endOfLine_type{l->spacing(i), l->spacingEolWidth(i), l->spacingEolWithin(i)};
                    }else{
                        return Lef::layer_type::endOfLine_type{0.0, 0.0, 0.0};
                    }
                    }(),
                    [&](){
                        if(l->spacingTable(0)->isParallel())
                        {
                            auto parallel = l->spacingTable(0)->parallel();
                            Lef::layer_type::parallelRunLength_type parallelRunLength = Lef::layer_type::parallelRunLength_type{parallel->numLength(), parallel->numWidth()};
                            for (int i = 0; i < parallel->numLength(); ++i) {
                                parallelRunLength.add_length(micrometer_type{parallel->length(i)});
                                for (int j = 0; j < parallel->numWidth(); ++j) {
                                   parallelRunLength.add_width(micrometer_type{parallel->width(j)});
                                   parallelRunLength.add_spacing(micrometer_type{parallel->width(j)}, micrometer_type{parallel->length(i)}, micrometer_type{parallel->widthSpacing(i, j)});
                                }
                            }
                            return parallelRunLength;
                        }else{
                            return Lef::layer_type::parallelRunLength_type{0, 0};
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
                m.m_name = l->name();
                m.m_class_name = (l->hasClass() ? l->macroClass() : "");
                if(l->hasForeign()) {
                    m.m_foreign.name = l->foreignName();
                    m.m_foreign.offset = Lef::macro_type::micrometer_point_type{
                        Lef::macro_type::micrometer_type{l->foreignX()},
                        Lef::macro_type::micrometer_type{l->foreignY()}
                    };
                }
                m.m_origin = Lef::macro_type::micrometer_point_type{
                    Lef::macro_type::micrometer_type{l->originX()},
                    Lef::macro_type::micrometer_type{l->originY()}
                };
                m.m_size = Lef::macro_type::micrometer_point_type{
                    Lef::macro_type::micrometer_type{l->sizeX()},
                    Lef::macro_type::micrometer_type{l->sizeY()}
                };
                m.m_site = (l->hasSiteName() ? l->siteName() : "");

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

                that->m_macros.back().m_pins.emplace_back(
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
                        that->m_macros.back().m_obstructions[last_layer].emplace_back(
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
                Lef::Via via = Lef::Via{l->name()};

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

    const Lef::Site::string_type& Lef::Site::name() const noexcept
    {
        return m_name;
    }

    const Lef::Site::string_type& Lef::Site::class_name() const noexcept
    {
        return m_class_name;
    }

    const Lef::Site::micrometer_type& Lef::Site::width() const noexcept
    {
        return m_width;
    }

    const Lef::Site::micrometer_type& Lef::Site::height() const noexcept
    {
        return m_height;
    }

    const Lef::Site::symmetry_type& Lef::Site::symmetry() const noexcept
    {
        return m_symmetry;
    }

    std::ostream& operator<<(std::ostream& os, const Lef::Site& site)
    {
        auto symmetry_string = [&](){
            using namespace std::literals;

            auto st = ""s;
            if(site.m_symmetry.is_x_symmetric){
                st += "X";
            }
            if(site.m_symmetry.is_y_symmetric){
                st += "Y";
            }
            if(site.m_symmetry.is_90_symmetric){
                st += "90";
            }

            return st;
        };

        os << "{name: " << site.m_name 
            << ", class: " << site.m_class_name
            << ", width: " << site.m_width
            << ", height: " << site.m_height
            << ", symmetry:" << symmetry_string() 
            << "}";

        return os;
    }

    bool Lef::Site::operator==(const Lef::Site& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_class_name == rhs.m_class_name &&
            m_width == rhs.m_width &&
            m_height == rhs.m_height &&
            m_symmetry == rhs.m_symmetry;
    }

    bool Lef::Site::Symmetry::operator==(const Lef::Site::Symmetry& rhs) const noexcept
    {
        return is_x_symmetric == rhs.is_x_symmetric &&
            is_y_symmetric == rhs.is_y_symmetric &&
            is_90_symmetric == rhs.is_90_symmetric;
    }

    const Lef::Layer::string_type& Lef::Layer::name() const noexcept
    {
        return m_name;
    }

    const Lef::Layer::type_type& Lef::Layer::type() const noexcept
    {
        return m_type;
    }

    const Lef::Layer::direction_type& Lef::Layer::direction() const noexcept
    {
        return m_direction;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::pitch() const noexcept
    {
        return m_pitch;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::offset() const noexcept
    {
        return m_offset;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::width() const noexcept
    {
        return m_width;
    }

    bool Lef::Layer::operator==(const Lef::Layer& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_type == rhs.m_type &&
            m_direction == rhs.m_direction &&
            m_pitch == rhs.m_pitch &&
            m_offset == rhs.m_offset &&
            m_width == rhs.m_width;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::min_width() const noexcept{
        return m_min_width;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::area() const noexcept{
        return m_area;
    }

    const Lef::Layer::micrometer_type& Lef::Layer::spacing() const noexcept{
        return m_spacing;
    }

    const Lef::Layer::endOfLine_type& Lef::Layer::EOL() const noexcept{
        return m_eol;
    }

    const Lef::Layer::parallelRunLength_type& Lef::Layer::parallelRunLength() const noexcept{
        return m_parallelRunLength;
    }

    std::ostream& operator<<(std::ostream& os, const Lef::Layer& layer)
    {
        auto type_string = [&](){
            switch(layer.m_type){
                case Lef::Layer::type_type::MASTERSLICE:
                    return "MASTERSLICE";
                case Lef::Layer::type_type::CUT:
                    return "CUT";
                case Lef::Layer::type_type::ROUTING:
                    return "ROUTING";
                default:
                    return "NOT_ASSIGNED";
            }
        };

        auto direction_string = [&](){
            switch(layer.m_direction){
                case Lef::Layer::direction_type::HORIZONTAL:
                    return "HORIZONTAL";
                case Lef::Layer::direction_type::VERTICAL:
                    return "VERTICAL";
                default:
                    return "NOT_ASSIGNED";
            }
        };

        os << "{name: " << layer.m_name 
            << ", type: " << type_string()
            << ", direction: " << direction_string()
            << ", pitch: " << layer.m_pitch
            << ", offset:" << layer.m_offset
            << ", width: " << layer.m_width
            << "}";

        return os;
    }

    const Lef::Macro::string_type& Lef::Macro::name() const noexcept
    {
        return m_name;
    }

    const Lef::Macro::string_type& Lef::Macro::class_name() const noexcept
    {
        return m_class_name;
    }

    const Lef::Macro::foreign_type& Lef::Macro::foreign() const noexcept
    {
        return m_foreign;
    }

    const Lef::Macro::micrometer_point_type& Lef::Macro::origin() const noexcept
    {
        return m_origin;
    }

    const Lef::Macro::micrometer_point_type& Lef::Macro::size() const noexcept
    {
        return m_size;
    }

    const Lef::Macro::string_type& Lef::Macro::site() const noexcept
    {
        return m_site;
    }

    const Lef::Macro::pin_container_type& Lef::Macro::pins() const noexcept
    {
        return m_pins;
    }

    const Lef::Macro::obstruction_map_type& Lef::Macro::obstructions() const noexcept
    {
        return m_obstructions;
    }

    const Lef::Macro::Pin::string_type& Lef::Macro::Pin::name() const noexcept
    {
        return m_name;
    }

    const Lef::Macro::Pin::direction_type& Lef::Macro::Pin::direction() const noexcept
    {
        return m_direction;
    }

    const Lef::Macro::Pin::port_map_type& Lef::Macro::Pin::ports() const noexcept
    {
        return m_ports;
    }

    const Lef::Layer::EndOfLine::micrometer_type & Lef::Layer::EndOfLine::space() const noexcept
    {
        return m_space;
    }

    const Lef::Layer::EndOfLine::micrometer_type & Lef::Layer::EndOfLine::width() const noexcept
    {
        return m_width;
    }

    const Lef::Layer::EndOfLine::micrometer_type & Lef::Layer::EndOfLine::within() const noexcept
    {
        return m_within;
    }

    void Lef::Layer::ParallelRunLength::add_length(Lef::Layer::ParallelRunLength::micrometer_type length) noexcept
    {
        m_lengths.push_back(length);
    }

    void Lef::Layer::ParallelRunLength::add_width(Lef::Layer::ParallelRunLength::micrometer_type width) noexcept
    {
        m_widths.push_back(width);
    }

    void Lef::Layer::ParallelRunLength::add_spacing(Lef::Layer::ParallelRunLength::micrometer_type width, Lef::Layer::ParallelRunLength::micrometer_type length, Lef::Layer::ParallelRunLength::micrometer_type spacing) noexcept
    {
        m_values.insert(std::make_pair(std::make_pair(width, length), spacing));
    }

    const Lef::Layer::ParallelRunLength::parallelRunLength_container_type& Lef::Layer::ParallelRunLength::values() const noexcept
    {
        return m_values;
    }

    const Lef::Layer::ParallelRunLength::micrometer_type &Lef::Layer::ParallelRunLength::spacing(const Lef::Layer::ParallelRunLength::micrometer_type width, const Lef::Layer::ParallelRunLength::micrometer_type length) const noexcept
    {
        return m_values.at(std::make_pair(width, length));
    }

    const Lef::Layer::ParallelRunLength::micrometer_container_type& Lef::Layer::ParallelRunLength::widths() const noexcept
    {
        return m_widths;
    }

    const Lef::Layer::ParallelRunLength::micrometer_container_type& Lef::Layer::ParallelRunLength::lengths() const noexcept
    {
        return m_lengths;
    }

    int Lef::Layer::ParallelRunLength::numWidth() const noexcept
    {
        return m_numWidth;
    }

    int Lef::Layer::ParallelRunLength::numLength() const noexcept
    {
        return m_numLength;
    }

    const Lef::Via::string_type& Lef::Via::name() const noexcept
    {
        return m_name;
    }

    void Lef::Via::addLayer(Lef::Via::string_type layer, layer_container_type boxes) noexcept
    {
        m_layers.insert(std::make_pair(layer, boxes));
    }

    const Lef::Via::layer_map_type &Lef::Via::layers() const noexcept
    {
        return m_layers;
    }

}     /* namespace parser */
}     /* namespace ophidian */
