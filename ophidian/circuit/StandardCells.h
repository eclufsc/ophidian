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
        // Class member types
        using size_type = std::size_t;
        using cell_type = Cell;
        using cell_name_type = std::string;
        using cell_const_iterator = entity_system::EntitySystem<cell_type>::const_iterator;

        using pin_type = Pin;
        using pin_name_type = std::string;
        using pin_direction_type = PinDirection;
        using pin_const_iterator = entity_system::EntitySystem<pin_type>::const_iterator;

        //! StandardCell Constructor
        StandardCells() = default;

        StandardCells(const StandardCells&) = delete;
        StandardCells& operator=(const StandardCells&) = delete;

        StandardCells(StandardCells&&) = default;
        StandardCells& operator=(StandardCells&&) = default;

        //--------------------------- Cells -------------------------------//

        //! Add Cell

        /*!
           \brief Adds a cell instance. A cell has a name associated to it. If the cell already exist then just return the existing cell.
           \param name Name of the cell, used to identify it.
           \return A handler for the created/existing Cell.
         */
        cell_type add(cell_type, const cell_name_type& name);

        //! Erase Cell

        /*!
           \param cell A handler for the Cell to erase.
           \brief Erases a Cell instance.
         */
        void erase(const cell_type& cell);

        //! Allocate space for storing Cell entities

        /*!
           \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions cells), then it is worth reserving space for this amount before starting to build the standard cells.
           \param size Minimum capacity for the cell container.
         */
        void reserve(cell_type, size_type size);

        //! Size of Cell's System

        /*!
           \brief Returns the number of Cells.
           \return The number of Cells.
         */
        size_type size(cell_type) const;

        //! Capacity of the Cell's System

        /*!
           \return The capacity of the Cell EntitySystem.
         */
        size_type capacity(cell_type) const;

        //! Find a cell

        /*!
           \brief Using the mapping, return a cell handler by cell's name.
           \param The cell name.
           \return Return a cell handler by cell's name.
         */
        cell_type find(cell_type, cell_name_type cellName);

        //! Cells iterator

        /*!
           \return Range iterator for the cells.
         */
        ophidian::util::Range<cell_const_iterator> range(cell_type) const;

        //! Cell name getter

        /*!
           \brief Get the name of a given cell.
           \param cell Cell entity to get the name.
           \return Name of the cell
         */
        cell_name_type name(const cell_type & cell) const;

        //! Pins of a Cell

        /*!
           \brief Returns a Container Wrapper for the Pins of a Cell.
           \param cell A handler for the Cell we want to get the Pins.
           \return Container Wrapper for the Pins of a Cell.
         */
        entity_system::Association<cell_type, pin_type>::Parts pins(const cell_type & cell) const;

        //! Make Cell Property

        /*!
           \brief Creates a Property for the Standard Cell's Entity System.
           \tparam Value value type of the Property.
           \return An Cell => \p Value Map.
         */
        template <typename Value>
        entity_system::Property<cell_type, Value> makeProperty(cell_type) const
        {
            return entity_system::Property<cell_type, Value>(mCells);
        }

        //--------------------------- Pins -------------------------------//

        //! Add Pin

        /*!
           \brief Adds a pin instance. A pin has a name and a direction associated to it.  If the pin already exist then just return the existing pin.
           \param name Name of the pin, used to identify it.
           \return A handler for the created/existing pin.
         */
        pin_type add(pin_type, const pin_name_type& name, PinDirection direction);

        //! Erase Pin

        /*!
           \param pin A handler for the pin to erase.
           \brief Erases a pin_type instance.
         */
        void erase(const pin_type & pin);

        //! Allocate space for storing pin entities

        /*!
           \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions pins), then it is worth reserving space for this amount before starting to build the standard cells.
           \param size Minimum capacity for the Pin container.
         */
        void reserve(pin_type, size_type size);

        //! Size of Pin's System

        /*!
           \brief Returns the number of pins.
           \return The number of pins.
         */
        size_type size(pin_type) const;

        //! Capacity of the Pin's System

        /*!
           \return The capacity of the Pin EntitySystem.
         */
        size_type capacity(pin_type) const;

        //! Find a pin

        /*!
           \brief Using the mapping, return a pin handler by pin's name.
           \param The pin name.
           \return Return a pin handler by pin's name.
         */
        pin_type find(pin_type, pin_name_type pinName);

        //! Pin name getter

        /*!
           \brief Get the name of a given pin.
           \param pin Pin entity to get the name.
           \return Name of the pin
         */
        pin_name_type name(const pin_type & pin) const;

        //! Pin direction getter

        /*!
           \brief Get the direction of a given pin. A direction can be NOT_ASSIGNED, INPUT, OUTPUT or INOUT.
           \param pin Pin entity to get the direction.
           \return Direction of the pin
         */
        PinDirection direction(const pin_type & pin);

        //! Pin owner getter

        /*!
           \brief Get the owner of a given pin.
           \param pin Pin entity to get the owner.
           \return Owner of the pin
         */
        cell_type owner(const pin_type & pin);

        //! Pins iterator

        /*!
           \return Range iterator for the Pins.
         */
        ophidian::util::Range<pin_const_iterator> range(pin_type) const;

        //! Make Pin Property

        /*!
           \brief Creates a Property for the Standard Cell's Entity System.
           \tparam Value value type of the Property.
           \return An Pin => \p Value Map.
         */
        template <typename Value>
        entity_system::Property<pin_type, Value> makeProperty(pin_type) const
        {
            return entity_system::Property<pin_type, Value>(mPins);
        }

        //--------------------------- Association -------------------------------//

        //! Add Pin into Cell

        /*!
           \brief Adds a Pin to a given Cell.
           \param cell A handler for the Cell we want to add a Pin.
           \param pin A handler for the Pin we want to add in \p cell.
         */

        //Maybe rename to create_association or associate...
        void add(const cell_type & cell, const pin_type & pin);

    private:

        //cells entity system and properties
        entity_system::EntitySystem<cell_type>          mCells{};
        entity_system::Property<cell_type, cell_name_type> mCellNames{mCells};

        //pins entity system and properties
        entity_system::EntitySystem<pin_type>           mPins{};
        entity_system::Property<pin_type, pin_name_type>  mPinNames{mPins};
        entity_system::Property<pin_type, PinDirection> mPinDirections{mPins};

        //composition and aggregation relations
        entity_system::Composition<cell_type, pin_type> mCellPins{mCells, mPins};

        //std_cell and pin mapping
        std::unordered_map<cell_name_type, cell_type> mName2Cell{};
        std::unordered_map<pin_name_type, pin_type>  mName2Pin{};
    };
}     //namespace ophidian
}     //namespace standard_cell

#endif // OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
