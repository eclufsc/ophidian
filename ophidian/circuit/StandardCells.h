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

#ifndef OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
#define OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H

#include <memory>
#include <unordered_map>
#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/entity_system/Composition.h>
#include <ophidian/util/Range.h>

namespace ophidian
{
namespace circuit
{
    class Pin :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Cell :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    enum class PinDirection {
        INPUT, OUTPUT, INOUT, NA
    };

    class StandardCells
    {
    public:
        // Member types
        using cell_type = Cell;
        using cell_name_type = std::string;
        using cell_container_type = entity_system::EntitySystem<cell_type>;

        using pin_type = Pin;
        using pin_name_type = std::string;
        using pin_direction_type = PinDirection;
        using pin_container_type = entity_system::EntitySystem<pin_type>;

        using cell_pins_view_type = entity_system::Association<cell_type, pin_type>::Parts;

        // Constructors
        StandardCells() = default;

        StandardCells(const StandardCells&) = delete;
        StandardCells& operator=(const StandardCells&) = delete;

        StandardCells(StandardCells&&) = default;
        StandardCells& operator=(StandardCells&&) = default;

        // Element access
        const cell_type& find_cell(cell_name_type cellName) const;

        const pin_type& find_pin(pin_name_type pinName) const;

        cell_name_type& name(const cell_type & cell);
        const cell_name_type& name(const cell_type & cell) const;

        pin_name_type& name(const pin_type & pin);
        const pin_name_type& name(const pin_type & pin) const;

        pin_direction_type& direction(const pin_type & pin);
        const pin_direction_type& direction(const pin_type & pin) const;

        cell_type cell(const pin_type & pin) const;

        cell_pins_view_type pins(const cell_type & cell) const;

        // Iterators
        ophidian::util::Range<cell_container_type::const_iterator> range_cell() const;
        ophidian::util::Range<pin_container_type::const_iterator> range_pin() const;

        // Capacity
        void reserve_cell(cell_container_type::size_type size);
        void reserve_pin(pin_container_type::size_type size);

        cell_container_type::size_type capacity_cell() const noexcept;

        pin_container_type::size_type capacity_pin() const noexcept;

        cell_container_type::size_type size_cell() const noexcept;

        pin_container_type::size_type size_pin() const noexcept;

        // Modifiers
        cell_type add_cell(const cell_name_type& name);
        pin_type add_pin(const pin_name_type& name, PinDirection direction);

        void connect(const cell_type & cell, const pin_type & pin);

        void erase(const cell_type& cell);
        void erase(const pin_type & pin);

        template <typename Value>
        entity_system::Property<cell_type, Value> make_property_cell() const
        {
            return entity_system::Property<cell_type, Value>(mCells);
        }

        template <typename Value>
        entity_system::Property<pin_type, Value> make_property_pin() const
        {
            return entity_system::Property<pin_type, Value>(mPins);
        }

    private:
        //cells entity system and properties
        cell_container_type                                mCells{};
        entity_system::Property<cell_type, cell_name_type> mCellNames{mCells};

        //pins entity system and properties
        pin_container_type                               mPins{};
        entity_system::Property<pin_type, pin_name_type> mPinNames{mPins};
        entity_system::Property<pin_type, PinDirection>  mPinDirections{mPins};

        //composition and aggregation relations
        entity_system::Composition<cell_type, pin_type> mCellPins{mCells, mPins};

        //std_cell and pin mapping
        std::unordered_map<cell_name_type, cell_type> mName2Cell{};
        std::unordered_map<pin_name_type, pin_type>   mName2Pin{};
    };
}     //namespace ophidian
}     //namespace standard_cell

#endif // OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
