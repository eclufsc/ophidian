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

#include <defrReader.hpp>

#include "Def.h"
#include "ParserException.h"

namespace ophidian::parser
{
    Def::Def(const std::string& def_file):
        m_die_area{},
        m_rows{},
        m_components{},
        m_nets{},
        m_dbu_to_micrometer_ratio{0}
    {
        read_file(def_file);
    }

    Def::Def(const std::vector<std::string>& def_files):
        m_die_area{},
        m_rows{},
        m_components{},
        m_nets{},
        m_dbu_to_micrometer_ratio{}
    {
        for(const auto& file : def_files){
            read_file(file);
        }
    }

    void Def::read_file(const std::string& def_file)
    {
        defrInit();
        defrSetUnitsCbk(
            [](defrCallbackType_e, double number, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);
                that->m_dbu_to_micrometer_ratio = Def::scalar_type{number};
                return 0;
            }
        );

        defrSetTrackCbk(
            [](defrCallbackType_e, defiTrack *track, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);

                that->m_tracks.emplace_back(
                    [&]() -> Def::track_type::orientation_type {
                        auto orientation_str = std::string(track->macro());
                        if(orientation_str == "X") { return Def::track_type::orientation_type::X; }
                        else { return Def::track_type::orientation_type::Y; }
                     }(),
                     Def::track_type::database_unit_type{static_cast<double>(track->x())},
                     Def::track_type::scalar_type{static_cast<double>(track->xNum())},
                     Def::track_type::database_unit_type{static_cast<double>(track->xStep())},
                     track->layer(0)
                     );
                return 0;
            }
        );

        defrSetDieAreaCbk(
            [](defrCallbackType_e, defiBox * box, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);
                that->m_die_area = Def::database_unit_box_type{
                    {
                        Def::database_unit_type{static_cast<double>(box->xl())},
                        Def::database_unit_type{static_cast<double>(box->yl())}
                    },
                    {
                        Def::database_unit_type{static_cast<double>(box->xh())},
                        Def::database_unit_type{static_cast<double>(box->yh())}
                    }
                };
                return 0;
            }
        );

        defrSetRowCbk(
            [](defrCallbackType_e, defiRow * defrow, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);

                that->m_rows.emplace_back(
                    defrow->name(),
                    defrow->macro(),
                    Def::row_type::database_unit_point_type{
                        Def::row_type::database_unit_type{defrow->x()},
                        Def::row_type::database_unit_type{defrow->y()}
                    },
                    Def::row_type::database_unit_point_type{
                        Def::row_type::database_unit_type{defrow->xStep()},
                        Def::row_type::database_unit_type{defrow->yStep()}
                    },
                    Def::row_type::scalar_point_type{
                        Def::row_type::scalar_type{defrow->xNum()},
                        Def::row_type::scalar_type{defrow->yNum()}
                    }
                );
                return 0;
            }
        );

        defrSetComponentStartCbk(
            [](defrCallbackType_e, int number, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);
                that->m_components.reserve(number);
                return 0;
            }
        );

        defrSetComponentCbk(
            [](defrCallbackType_e, defiComponent * comp, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);

                that->m_components.emplace_back(
                    comp->id(),
                    comp->name(),
                    [&]() -> Def::component_type::orientation_type {
                        auto orientation_str = comp->placementOrientStr();
                        if(orientation_str == "N")      { return Def::component_type::orientation_type::N; }
                        else if(orientation_str == "S") { return Def::component_type::orientation_type::S; }
                        else if(orientation_str == "W") { return Def::component_type::orientation_type::W; }
                        else if(orientation_str == "E") { return Def::component_type::orientation_type::E; }
                        else if(orientation_str == "FN"){ return Def::component_type::orientation_type::FN; }
                        else if(orientation_str == "FS"){ return Def::component_type::orientation_type::FS; }
                        else if(orientation_str == "FW"){ return Def::component_type::orientation_type::FW; }
                        else if(orientation_str == "FE"){ return Def::component_type::orientation_type::FE; }
                        else { return Def::component_type::orientation_type::N; }
                    }(),
                    Def::component_type::database_unit_point_type{
                        Def::component_type::database_unit_type{static_cast<double>(comp->placementX())},
                        Def::component_type::database_unit_type{static_cast<double>(comp->placementY())}
                    },
                    comp->isFixed()
                );
                return 0;
            }
        );

        defrSetNetCbk(
            [](defrCallbackType_e, defiNet *net, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);

                auto pins = Def::net_type::pin_container_type{};

                for (int i = 0; i < net->numConnections(); ++i) {
                    pins.emplace_back(net->instance(i), net->pin(i));
                }

                that->m_nets.emplace_back(net->name(), std::move(pins));
                return 0;
            }
        );

        auto fp = std::unique_ptr<FILE, decltype( & std::fclose)>(
            std::fopen(def_file.c_str(), "r"),
            &std::fclose);

        if(!fp) {
            throw exceptions::InexistentFile();
        }

        defrRead(fp.get(), def_file.c_str(), this, true);
        defrClear();
    }

    const Def::database_unit_box_type& Def::die_area() const noexcept
    {
        return m_die_area;
    }

    const Def::component_container_type& Def::components() const noexcept
    {
        return m_components;
    }

    const Def::net_container_type& Def::nets() const noexcept
    {
        return m_nets;
    }

    const Def::row_container_type& Def::rows() const noexcept
    {
        return m_rows;
    }

    const Def::scalar_type& Def::dbu_to_micrometer_ratio() const noexcept
    {
        return m_dbu_to_micrometer_ratio;
    }

    const Def::track_container_type& Def::tracks() const noexcept
    {
        return m_tracks;
    }
}
