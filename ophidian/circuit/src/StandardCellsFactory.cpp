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

#include <ophidian/circuit/StandardCellsFactory.h>

namespace ophidian::circuit::factory
{
    void make_standard_cells(StandardCells& cells, const parser::Lef& lef) noexcept
    {
        for(const auto& macro : lef.macros())
        {
            auto cell = cells.add_cell(macro.name());
            for(const auto& pin : macro.pins())
            {
                auto cell_pin = Pin{};
                auto pin_name = macro.name() + ":" + pin.name();
                switch(pin.direction())
                {
                    case parser::Lef::macro_type::pin_type::direction_type::INPUT:
                        cell_pin = cells.add_pin(pin_name, PinDirection::INPUT);
                        break;
                    case parser::Lef::macro_type::pin_type::direction_type::OUTPUT:
                        cell_pin = cells.add_pin(pin_name, PinDirection::OUTPUT);
                        break;
                    case parser::Lef::macro_type::pin_type::direction_type::INOUT:
                        cell_pin = cells.add_pin(pin_name, PinDirection::INOUT);
                        break;
                    case parser::Lef::macro_type::pin_type::direction_type::NA:
                        cell_pin = cells.add_pin(pin_name, PinDirection::NA);
                        break;
                    default:
                        cell_pin = cells.add_pin(pin_name, PinDirection::NA);
                        break;
                }
                cells.connect(cell, cell_pin);
            }
        }
    }

    void make_standard_cells(StandardCells& cells, const parser::ICCAD2020 & iccad_2020) noexcept {
        for(const auto& macro : iccad_2020.macros())
        {
            auto cell = cells.add_cell(macro.name());
            for(const auto& pin : macro.pins())
            {
                auto cell_pin = Pin{};
                auto pin_name = macro.name() + ":" + pin.name();
                cell_pin = cells.add_pin(pin_name, PinDirection::NA);
                cells.connect(cell, cell_pin);
            }
        }
    }
}
