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

#include "Lef2Library.h"

namespace ophidian
{
namespace placement
{
    void lef2Library(
        const parser::Lef & lef,
        Library & library,
        circuit::StandardCells & stdCells)
    {
        for(auto& macro : lef.macros())
        {
            auto stdCell = stdCells.add(circuit::Cell(), macro.name());
            auto layer2RectsM1 = macro.obstructions().find("metal1");
            if(layer2RectsM1 != macro.obstructions().end()) {
                geometry::MultiBox<util::database_unit_t> geometry;
                for(auto& rect : layer2RectsM1->second)
                {
                    geometry::Point<util::database_unit_t> pmin =
                    {rect.min_corner().x() * lef.micrometer_to_dbu_ratio(),
                     rect.min_corner().y() * lef.micrometer_to_dbu_ratio()};
                    geometry::Point<util::database_unit_t> pmax =
                    {rect.max_corner().x() * lef.micrometer_to_dbu_ratio(),
                     rect.max_corner().y() * lef.micrometer_to_dbu_ratio()};
                    geometry.push_back(geometry::Box<util::database_unit_t>(pmin, pmax));
                }
                library.geometry(stdCell, geometry);
            }
            else {
                geometry::Point<util::database_unit_t> pmin =
                    {
                        macro.origin().x() * lef.micrometer_to_dbu_ratio(),
                        macro.origin().y() * lef.micrometer_to_dbu_ratio()
                    };
                geometry::Point<util::database_unit_t> pmax =
                    {
                        macro.size().x() * lef.micrometer_to_dbu_ratio(),
                        macro.size().y() * lef.micrometer_to_dbu_ratio()
                    };
                library.geometry(stdCell,
                    geometry::MultiBox<util::database_unit_t>{std::vector<geometry::Box<util::database_unit_t>>{geometry::Box<util::database_unit_t>{pmin, pmax}}});
            }
            util::DbuConverter dbuConverter{lef.micrometer_to_dbu_ratio()};

            for(auto& pin : macro.pins())
            {
                auto stdPin = stdCells.add(circuit::Pin(),
                    macro.name() + ":" + pin.name(),
                    circuit::PinDirection(pin.direction()));
                stdCells.add(stdCell, stdPin);
                for(auto& port : pin.ports())
                {
                    for(auto& rect : port.second)
                    {
                        library.pinOffset(
                            stdPin,
                            util::LocationDbu(0.5 *
                                (dbuConverter.convert(rect.min_corner().x()) +
                                 dbuConverter.convert(rect.max_corner().x())),
                                0.5 *
                                (dbuConverter.convert(rect.min_corner().y()) +
                                 dbuConverter.convert(rect.max_corner().y()))));
                    }
                }
            }
        }
    }
}     // namespace placement
}     // namespace ophidian
