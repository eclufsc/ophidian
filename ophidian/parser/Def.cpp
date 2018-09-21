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

                using size_type = Def::net_type::pin_container_type::size_type;

                auto pins = Def::net_type::pin_container_type{static_cast<size_type>(net->numConnections())};

                for (int i = 0; i < net->numConnections(); ++i) {
                    pins.emplace_back(net->instance(i), net->pin(i));
                }

                that->m_nets.emplace_back(net->name(), pins);
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

    const Def::Component::string_type& Def::Component::name() const noexcept
    {
        return m_name;
    }

    const Def::Component::string_type& Def::Component::macro() const noexcept
    {
        return m_macro;
    }

    const Def::Component::orientation_type& Def::Component::orientation() const noexcept
    {
        return m_orientation;
    }

    const Def::Component::database_unit_point_type& Def::Component::position() const noexcept
    {
        return m_position;
    }

    bool Def::Component::fixed() const noexcept
    {
        return m_fixed;
    }

    bool Def::Component::operator==(const Def::Component& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_macro == rhs.m_macro &&
            m_orientation == rhs.m_orientation &&
            m_position.x() == rhs.m_position.x() && 
            m_position.y() == rhs.m_position.y() && 
            m_fixed == rhs.m_fixed;
    }

    std::ostream& operator<<(std::ostream& os, const Def::Component& component)
    {
        auto orientation_string = [&]() -> std::string {
            switch(component.m_orientation){
                case Def::Component::orientation_type::N:
                    return "N";
                case Def::Component::orientation_type::S:
                    return "S";
                case Def::Component::orientation_type::W:
                    return "W";
                case Def::Component::orientation_type::E:
                    return "E";
                case Def::Component::orientation_type::FN:
                    return "FN";
                case Def::Component::orientation_type::FS:
                    return "FS";
                case Def::Component::orientation_type::FW:
                    return "FW";
                case Def::Component::orientation_type::FE:
                    return "FE";
                default:
                    return "NA";
            }
        };

        auto fixed_string = [&]() -> std::string {
            return component.m_fixed ? "true" : "false";
        };

        os << "{name: " << component.m_name 
            << ", macro: " << component.m_macro
            << ", orientarion: " << orientation_string()
            << ", position: (" << component.m_position.x() 
            << ", " << component.m_position.y() << ")"
            << ", fixed: " << fixed_string() 
            << "}";

        return os;
    }

    const Def::Row::string_type& Def::Row::name() const noexcept
    {
        return m_name;
    }

    const Def::Row::string_type& Def::Row::site() const noexcept
    {
        return m_site;
    }

    const Def::Row::database_unit_point_type& Def::Row::origin() const noexcept
    {
        return m_origin;
    }

    const Def::Row::database_unit_point_type& Def::Row::step() const noexcept
    {
        return m_step;
    }

    const Def::Row::scalar_point_type& Def::Row::num() const noexcept
    {
        return m_num;
    }

    bool Def::Row::operator==(const Def::Row& rhs) const noexcept
    {
        return m_name == rhs.m_name &&
            m_site == rhs.m_site &&
            m_origin.x() == rhs.m_origin.x() &&
            m_origin.y() == rhs.m_origin.y() &&
            m_step.x() == rhs.m_step.x() &&
            m_step.y() == rhs.m_step.y() &&
            m_num.x() == rhs.m_num.x() &&
            m_num.y() == rhs.m_num.y();
    }

    std::ostream& operator<<(std::ostream& os,const Def::Row& row)
    {
        os << "{name: " << row.m_name 
            << ", site: " << row.m_site
            << ", origin: )" << row.m_origin.x()
            << ", " << row.m_origin.y() << ")"
            << ", step: (" << row.m_step.x() 
            << ", " << row.m_step.y() << ")"
            << ", num: (" << row.m_num.x() 
            << ", " << row.m_num.y() << ")"
            << "}";

        return os;
    }
}
