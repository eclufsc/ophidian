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

#include <memory>

#include <defrReader.hpp>

#include "Def.h"
#include "ParserException.h"

namespace ophidian
{
namespace parser
{
    Def::Def(const std::string& filename):
        m_die_area(),
        m_rows(),
        m_components(),
        m_dbu_to_micrometer_ratio(0)
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
                that->m_die_area = Def::box_dbu{
                    Def::point_dbu{Def::dbu_type{box->xl()}, Def::dbu_type{box->yl()}}, 
                    Def::point_dbu{Def::dbu_type{box->xh()}, Def::dbu_type{box->yh()}}
                };
                return 0;
            }
        );
        defrSetRowCbk(
            [](defrCallbackType_e, defiRow * defrow, defiUserData ud) -> int {
                auto that = static_cast<Def *>(ud);
                auto row = Def::Row{
                    defrow->name(),
                    defrow->macro(),
                    {dbu_type{defrow->x()}, dbu_type{defrow->y()}},
                    {dbu_type{defrow->xStep()}, dbu_type{defrow->yStep()}},
                    {scalar_type{defrow->xNum()},scalar_type{defrow->yNum()}},
                };
                that->m_rows.push_back(std::move(row));
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

                auto component = Def::Component{
                    comp->id(),
                    comp->name(),
                    [&]() -> Def::Component::Orientation {
                        auto orientation_str = comp->placementOrientStr();
                        if(orientation_str == "N")      { return Def::Component::Orientation::N; }
                        else if(orientation_str == "S") { return Def::Component::Orientation::S; }
                        else if(orientation_str == "W") { return Def::Component::Orientation::W; }
                        else if(orientation_str == "E") { return Def::Component::Orientation::E; }
                        else if(orientation_str == "FN"){ return Def::Component::Orientation::FN; }
                        else if(orientation_str == "FS"){ return Def::Component::Orientation::FS; }
                        else if(orientation_str == "FW"){ return Def::Component::Orientation::FW; }
                        else if(orientation_str == "FE"){ return Def::Component::Orientation::FE; }
                        else { return Def::Component::Orientation::N; }
                    }(),
                    {Def::dbu_type{comp->placementX()},Def::dbu_type{comp->placementY()}},
                    comp->isFixed()
                };

                that->m_components.push_back(component);
                return 0;
            }
        );

        if(FILE * ifp = fopen(filename.c_str(), "r")) {
            auto res = defrRead(ifp, filename.c_str(), this, true);
            fclose(ifp);
            defrClear();
        }
        else {
            throw InexistentFile();
        }
    }

    const Def::box_dbu & Def::die_area() const noexcept
    {
        return m_die_area;
    }

    const Def::container_type<Def::Component> & Def::components() const noexcept
    {
        return m_components;
    }

    const Def::container_type<Def::Row> & Def::rows() const noexcept
    {
        return m_rows;
    }

    const Def::scalar_type Def::dbu_to_micrometer_ratio() const noexcept
    {
        return m_dbu_to_micrometer_ratio;
    }
}     // namespace parser
}     // namespace ophidian
