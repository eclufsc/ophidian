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
    using namespace std::literals;

    Lef::Lef(const std::string& filename):
        m_sites(),
        m_layers(),
        m_macros(),
        m_micrometer_to_dbu_ratio(0)
    {
        lefrInit();
        lefrSetUnitsCbk(
            [](lefrCallbackType_e, lefiUnits * units, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);
                that->m_micrometer_to_dbu_ratio = Lef::scalar_type{units->databaseNumber()};
                return 0;
            });
        lefrSetSiteCbk(
            [](lefrCallbackType_e, lefiSite * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto site = Lef::Site{
                    l->name(),
                    [&](){ return (l->hasClass() ? l->siteClass() : ""); }(),
                    Lef::micrometer_type{l->sizeX()},
                    Lef::micrometer_type{l->sizeY()},
                    {l->hasXSymmetry(), l->hasYSymmetry(), l->has90Symmetry()}
                };

                that->m_sites.push_back(site);
                return 0;
            });
        lefrSetLayerCbk(
            [](lefrCallbackType_e, lefiLayer * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto layer = Lef::Layer{
                    l->name(),
                    [&](){
                        if(l->hasType()){
                            auto type_name = std::string{l->type()};
                            if (type_name == "MASTERSLICE"){
                                return Lef::Layer::Type::MASTERSLICE;
                            }
                            else if(type_name == "CUT"){
                                return Lef::Layer::Type::CUT;
                            }
                            else if(type_name == "ROUTING"){
                                return Lef::Layer::Type::ROUTING;
                            }
                        }
                    }(),
                    [&](){
                        if(l->hasDirection()){
                            auto direction_name = std::string(l->direction());
                            if((direction_name == "HORIZONTAL") ||
                               (direction_name == "horizontal"))
                            {
                                return Lef::Layer::Direction::HORIZONTAL;
                            }
                            else if((direction_name == "VERTICAL") ||
                                    (direction_name == "vertical"))
                            {
                                return Lef::Layer::Direction::VERTICAL;
                            }
                        }
                        else {
                            return Lef::Layer::Direction::NOT_ASSIGNED; 
                        }
                    }(),
                    Lef::micrometer_type{l->pitch()},
                    Lef::micrometer_type{l->offset()},
                    Lef::micrometer_type{l->width()}
                };

                that->m_layers.push_back(layer);
                return 0;
            });

        lefrSetMacroBeginCbk(
            [](lefrCallbackType_e, const char * macro_name, lefiUserData ud) -> int {
                static_cast<Lef *>(ud)->m_macros.push_back(Lef::Macro{std::string{macro_name}});

                return 0;
            });

        lefrSetPinCbk(
            [](lefrCallbackType_e, lefiPin * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto port = Lef::Macro::Pin::Port{};

                for(int i = 0; i < l->numPorts(); ++i)
                {
                    auto rect = Lef::box_micrometer{};
                    auto layer_name = "none"s;
                    for(int j = 0; j < l->port(i)->numItems(); ++j)
                    {
                        switch(l->port(i)->itemType(j))
                        {
                        case lefiGeomLayerE:
                            layer_name = l->port(i)->getLayer(j);
                            break;

                        case lefiGeomRectE:
                            rect = Lef::box_micrometer{
                                Lef::point_micrometer{
                                    Lef::micrometer_type{l->port(i)->getRect(j)->xl},
                                    Lef::micrometer_type{l->port(i)->getRect(j)->yl}
                                }, Lef::point_micrometer{
                                    Lef::micrometer_type{l->port(i)->getRect(j)->xh},
                                    Lef::micrometer_type{l->port(i)->getRect(j)->yh}
                                }
                            };
                            port.layer2rects[layer_name].push_back(rect);
                            break;
                        }
                    }
                }

                auto pin = Lef::Macro::Pin{
                    l->name(),
                    [&](){
                        if(l->hasDirection()) {
                            if(std::string(l->direction()) == "INPUT") {
                                return Lef::Macro::Pin::Direction::INPUT;
                            }
                            else if(std::string(l->direction()) == "OUTPUT") {
                                return Lef::Macro::Pin::Direction::OUTPUT;
                            }
                            else if(std::string(l->direction()) == "INOUT") {
                                return Lef::Macro::Pin::Direction::INOUT;
                            }
                        }
                        else {
                            return Lef::Macro::Pin::Direction::NA;
                        }
                    }(),
                    port
                };

                that->m_macros.back().pins.push_back(pin);
                return 0;
            });

        lefrSetObstructionCbk(
            [](lefrCallbackType_e, lefiObstruction * l, lefiUserData ud) -> int {
                auto that = static_cast<Lef *>(ud);

                auto geometries = l->geometries();

                std::string last_layer;
                for(int i = 0; i < geometries->numItems(); ++i)
                {
                    switch(geometries->itemType(i))
                    {
                    case lefiGeomLayerE:
                        last_layer = geometries->getLayer(i);
                        break;

                    case lefiGeomRectE:
                        auto geom_rect = geometries->getRect(i);
                        auto rect = Lef::box_micrometer{
                            Lef::point_micrometer{
                                Lef::micrometer_type{geom_rect->xl},
                                Lef::micrometer_type{geom_rect->yl}
                            }, Lef::point_micrometer{
                                Lef::micrometer_type{geom_rect->xh}, 
                                Lef::micrometer_type{geom_rect->yh}
                            }
                        };
                        that->m_macros.back().obstructions.layer2rects[last_layer].push_back(rect);
                        break;
                    }
                }

                return 0;
            });

        lefrSetMacroCbk(
            [](lefrCallbackType_e, lefiMacro * l, lefiUserData ud) -> int {
                auto& m = static_cast<Lef *>(ud)->m_macros.back();
                m.name = l->name();
                m.class_name = (l->hasClass() ? l->macroClass() : "");
                m.origin = Lef::point_micrometer{
                    Lef::micrometer_type{l->originX()},
                    Lef::micrometer_type{l->originY()}
                };
                if(l->hasForeign()) {
                    m.foreign.name = l->foreignName();
                    m.foreign.x_offset = Lef::micrometer_type{l->foreignX()};
                    m.foreign.y_offset = Lef::micrometer_type{l->foreignY()};
                }
                m.size = Lef::Macro::Macro_size {
                    Lef::micrometer_type{l->sizeX()},
                    Lef::micrometer_type{l->sizeY()}
                };
                m.site = (l->hasSiteName() ? l->siteName() : "");

                return 0;
            });


        lefrSetMacroEndCbk(
            [](lefrCallbackType_e, const char * string, lefiUserData) -> int {
                return 0;
            });

        auto fp = std::unique_ptr<FILE, decltype( & std::fclose)>(
            std::fopen(filename.c_str(), "r"),
            &std::fclose);

        if(!fp) {
            throw InexistentFile();
        }

        auto res = lefrRead(fp.get(), filename.c_str(), this);
    }

    const Lef::container_type<Lef::Site> & Lef::sites() const noexcept
    {
        return m_sites;
    }

    const Lef::container_type<Lef::Layer> & Lef::layers() const noexcept
    {
        return m_layers;
    }

    const Lef::container_type<Lef::Macro> & Lef::macros() const noexcept
    {
        return m_macros;
    }

    const Lef::scalar_type& Lef::micrometer_to_dbu_ratio() const noexcept
    {
        return m_micrometer_to_dbu_ratio;
    }

    bool Lef::Site::Symmetry::operator ==(const Lef::Site::Symmetry& rhs) const {
        return is_x_symmetric == rhs.is_x_symmetric &&
            is_y_symmetric == rhs.is_y_symmetric &&
            is_90_symmetric == rhs.is_90_symmetric;
    }

    bool Lef::Site::operator ==(const Lef::Site& rhs) const {
        return name == rhs.name &&
            class_name == rhs.class_name &&
            width == rhs.width &&
            height == rhs.height &&
            symetry == rhs.symetry;
    }

    bool Lef::Layer::operator ==(const Lef::Layer& rhs) const {
        return name == rhs.name &&
            type == rhs.type &&
            direction == rhs.direction &&
            pitch == rhs.pitch &&
            offset == rhs.offset &&
            width == rhs.width;
    }
}     /* namespace parser */
}     /* namespace ophidian */
