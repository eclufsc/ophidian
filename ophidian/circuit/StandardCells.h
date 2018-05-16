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
        using size_type = std::size_t;
        using cell_type = Cell;
        using cell_name_type = std::string;
        using cell_const_iterator = entity_system::EntitySystem<cell_type>::const_iterator;

        using pin_type = Pin;
        using pin_name_type = std::string;
        using pin_direction_type = PinDirection;
        using pin_const_iterator = entity_system::EntitySystem<pin_type>::const_iterator;

        // Constructors
        StandardCells() = default;

        StandardCells(const StandardCells&) = delete;
        StandardCells& operator=(const StandardCells&) = delete;

        StandardCells(StandardCells&&) = default;
        StandardCells& operator=(StandardCells&&) = default;

        // Element access
        cell_type find(cell_type, cell_name_type cellName);
        pin_type find(pin_type, pin_name_type pinName);

        cell_name_type name(const cell_type & cell) const;
        pin_name_type name(const pin_type & pin) const;
        PinDirection direction(const pin_type & pin);

        cell_type owner(const pin_type & pin);

        entity_system::Association<cell_type, pin_type>::Parts pins(const cell_type & cell) const;

        // Iterators
        ophidian::util::Range<cell_const_iterator> range(cell_type) const;
        ophidian::util::Range<pin_const_iterator> range(pin_type) const;

        // Capacity
        void reserve(cell_type, size_type size);
        void reserve(pin_type, size_type size);

        size_type capacity(cell_type) const;
        size_type capacity(pin_type) const;

        size_type size(cell_type) const;
        size_type size(pin_type) const;

        // Modifiers
        cell_type add(cell_type, const cell_name_type& name);
        pin_type add(pin_type, const pin_name_type& name, PinDirection direction);

        void add(const cell_type & cell, const pin_type & pin);

        void erase(const cell_type& cell);
        void erase(const pin_type & pin);
        
        template <typename Value>
        entity_system::Property<cell_type, Value> makeProperty(cell_type) const
        {
            return entity_system::Property<cell_type, Value>(mCells);
        }

        template <typename Value>
        entity_system::Property<pin_type, Value> makeProperty(pin_type) const
        {
            return entity_system::Property<pin_type, Value>(mPins);
        }

    private:
        //cells entity system and properties
        entity_system::EntitySystem<cell_type>             mCells{};
        entity_system::Property<cell_type, cell_name_type> mCellNames{mCells};

        //pins entity system and properties
        entity_system::EntitySystem<pin_type>            mPins{};
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
