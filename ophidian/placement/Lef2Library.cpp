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
        standard_cell::StandardCells & stdCells)
    {
        for(auto & macro : lef.macros())
        {
            auto stdCell = stdCells.add(standard_cell::Cell(), macro.name);
            auto layer2RectsM1 = macro.obstructions.layer2rects.find("metal1");
            if(layer2RectsM1 != macro.obstructions.layer2rects.end()) {
                geometry::MultiBox geometry;
                for(auto & rect : layer2RectsM1->second)
                {
                    geometry::Point<util::database_unit_t> pmin =
                    {rect.firstPoint.x() * lef.micron_to_dbu_convertion_factor(),
                     rect.firstPoint.y() * lef.micron_to_dbu_convertion_factor()};
                    geometry::Point<util::database_unit_t> pmax =
                    {rect.secondPoint.x() * lef.micron_to_dbu_convertion_factor(),
                     rect.secondPoint.y() * lef.micron_to_dbu_convertion_factor()};
                    geometry.push_back(ophidian::geometry::Box(pmin, pmax));
                }
                library.geometry(stdCell, geometry);
            }
            else {
                ophidian::geometry::Point pmin =
                {macro.origin.x * lef.micron_to_dbu_convertion_factor(), macro.origin.y * lef.micron_to_dbu_convertion_factor()};
                ophidian::geometry::Point pmax =
                {macro.size.x * lef.micron_to_dbu_convertion_factor(), macro.size.y * lef.micron_to_dbu_convertion_factor()};
                library.geometry(stdCell,
                    geometry::MultiBox({ophidian::geometry::Box(pmin, pmax)}));
            }
            util::DbuConverter dbuConverter(lef.micron_to_dbu_convertion_factor());

            for(auto pin : macro.pins)
            {
                auto stdPin = stdCells.add(standard_cell::Pin(),
                    macro.name + ":" + pin.name,
                    standard_cell::PinDirection(pin.direction));
                stdCells.add(stdCell, stdPin);
                for(auto port : pin.ports)
                {
                    for(auto rect : port.rects)
                    {
                        library.pinOffset(
                            stdPin,
                            util::LocationDbu(0.5 *
                                (dbuConverter.convert(rect.firstPoint.x()) +
                                 dbuConverter.convert(rect.secondPoint.x())),
                                0.5 *
                                (dbuConverter.convert(rect.firstPoint.y()) +
                                 dbuConverter.convert(rect.secondPoint.y()))));
                    }
                }
            }
        }
    }
}     // namespace placement
}     // namespace ophidian
