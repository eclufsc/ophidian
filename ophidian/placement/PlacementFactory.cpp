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

#include "PlacementFactory.h"

namespace ophidian::placement::factory
{
    void make_placement(Placement& placement, const parser::Def & def, const circuit::Netlist& netlist) noexcept
    {
        for(const auto & component : def.components())
        {
            auto cell = netlist.find_cell_instance(component.name());
            Placement::Orientation orientation;
            switch (component.orientation())
            {
                case ophidian::parser::Component::Orientation::N :
                    orientation = Placement::Orientation::N;
                    break;
                case ophidian::parser::Component::Orientation::S :
                    orientation = Placement::Orientation::S;
                    break;
                case ophidian::parser::Component::Orientation::W :
                    orientation = Placement::Orientation::W;
                    break;
                case ophidian::parser::Component::Orientation::E :
                    orientation = Placement::Orientation::E;
                    break;
                case ophidian::parser::Component::Orientation::FN :
                    orientation = Placement::Orientation::FN;
                    break;
                case ophidian::parser::Component::Orientation::FS :
                    orientation = Placement::Orientation::FS;
                    break;
                case ophidian::parser::Component::Orientation::FW :
                    orientation = Placement::Orientation::FW;
                    break;
                case ophidian::parser::Component::Orientation::FE :
                    orientation = Placement::Orientation::FE;
                    break;
                default:
                    orientation = Placement::Orientation::N;
            }
            placement.place(cell, component.position(), orientation);
            if(component.fixed())
                placement.fixLocation(cell);
            else
                placement.unfixLocation(cell);
        }
    }
}
