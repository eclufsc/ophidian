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

#include <ophidian/circuit/StandardCells.h>

namespace ophidian::circuit
{
    StandardCells::cell_type StandardCells::find_cell(StandardCells::cell_name_type cellName) const
    {
        return m_name_to_std_cell.at(cellName);
    }

    StandardCells::pin_type StandardCells::find_pin(StandardCells::pin_name_type pinName) const
    {
        return m_name_to_std_pin.at(pinName);
    }

    StandardCells::cell_name_type& StandardCells::name(const StandardCells::cell_type& cell)
    {
        return m_std_cell_names[cell];
    }

    const StandardCells::cell_name_type& StandardCells::name(const StandardCells::cell_type& cell) const
    {
        return m_std_cell_names[cell];
    }

    StandardCells::pin_name_type& StandardCells::name(const StandardCells::pin_type& pin)
    {
        return m_std_pin_names[pin];
    }

    const StandardCells::pin_name_type& StandardCells::name(const StandardCells::pin_type& pin) const
    {
        return m_std_pin_names[pin];
    }

    PinDirection& StandardCells::direction(const StandardCells::pin_type& pin)
    {
        return m_std_pin_directions[pin];
    }

    const PinDirection& StandardCells::direction(const StandardCells::pin_type& pin) const
    {
        return m_std_pin_directions[pin];
    }

    StandardCells::cell_type StandardCells::cell(const StandardCells::pin_type& pin) const
    {
        return m_std_cell_to_std_pins.whole(pin);
    }

    entity_system::Association<StandardCells::cell_type, StandardCells::pin_type>::Parts StandardCells::pins(const StandardCells::cell_type& cell) const
    {
        return m_std_cell_to_std_pins.parts(cell);
    }

    ophidian::util::Range<StandardCells::cell_container_type::const_iterator> StandardCells::range_cell() const
    {
        return ophidian::util::Range<StandardCells::cell_container_type::const_iterator>(m_std_cells.begin(), m_std_cells.end());
    }

    ophidian::util::Range<StandardCells::pin_container_type::const_iterator> StandardCells::range_pin() const
    {
        return ophidian::util::Range<StandardCells::pin_container_type::const_iterator>(m_std_pins.begin(), m_std_pins.end());
    }

    void StandardCells::reserve_cell(StandardCells::cell_container_type::size_type size)
    {
        m_std_cells.reserve(size);
    }

    void StandardCells::reserve_pin(StandardCells::pin_container_type::size_type size)
    {
        m_std_pins.reserve(size);
    }

    StandardCells::cell_container_type::size_type StandardCells::capacity_cell() const noexcept
    {
        return m_std_cells.capacity();
    }

    StandardCells::pin_container_type::size_type StandardCells::capacity_pin() const noexcept
    {
        return m_std_pins.capacity();
    }

    StandardCells::cell_container_type::size_type StandardCells::size_cell() const noexcept
    {
        return m_std_cells.size();
    }

    StandardCells::pin_container_type::size_type StandardCells::size_pin() const noexcept
    {
        return m_std_pins.size();
    }

    StandardCells::cell_type StandardCells::add_cell(const StandardCells::cell_name_type& name)
    {
        if(m_name_to_std_cell.find(name) == m_name_to_std_cell.end()) {
            auto cell = m_std_cells.add();
            m_std_cell_names[cell] = name;
            m_name_to_std_cell[name] = cell;

            return cell;
        }
        else {
            return m_name_to_std_cell[name];
        }
    }

    StandardCells::pin_type StandardCells::add_pin(const StandardCells::pin_name_type& name, PinDirection direction)
    {
        if(m_name_to_std_pin.find(name) == m_name_to_std_pin.end()) {
            auto pin = m_std_pins.add();
            m_std_pin_names[pin] = name;
            m_std_pin_directions[pin] = direction;
            m_name_to_std_pin[name] = pin;

            return pin;
        }
        else {
            return m_name_to_std_pin[name];
        }
    }

    void StandardCells::connect(const StandardCells::cell_type& cell, const StandardCells::pin_type& pin)
    {
        m_std_cell_to_std_pins.addAssociation(cell, pin);
    }

    void StandardCells::erase(const StandardCells::cell_type& cell)
    {
        m_name_to_std_cell.erase(name(cell));
        m_std_cells.erase(cell);
    }

    void StandardCells::erase(const StandardCells::pin_type& pin)
    {
        m_name_to_std_pin.erase(name(pin));
        m_std_pins.erase(pin);
    }
}
