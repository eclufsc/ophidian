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
namespace standard_cell
{

class Pin : public entity_system::EntityBase
{
public:
	using entity_system::EntityBase::EntityBase;
};

class Cell : public entity_system::EntityBase
{
public:
	using entity_system::EntityBase::EntityBase;
};

enum class PinDirection
{
	INPUT, OUTPUT, INOUT, NA
};

class StandardCells
{
public:
	using CellsIterator = entity_system::EntitySystem<Cell>::const_iterator;
	using PinsIterator = entity_system::EntitySystem<Pin>::const_iterator;

	//! StandardCell Constructor
	/*!
	   \brief Constructs an empty system with no Cells and Pins.
	 */
	StandardCells();

	//! StandardCell Move Constructor
	/*!
	    \brief Move the entity system and its properties.
	 */
	StandardCells(const StandardCells && stdCell);

	//! StandardCell Destructor
	/*!
	   \brief Destroys the Cells and Pins EntitySystem, including its properties.
	 */
	~StandardCells();

//--------------------------- Cells -------------------------------//

	//! Add Cell
	/*!
	   \brief Adds a cell instance. A cell has a name associated to it. If the cell already exist then just return the existing cell.
	   \param name Name of the cell, used to identify it.
	   \return A handler for the created/existing Cell.
	 */
	Cell add(Cell, const std::string & name);

	//! Erase Cell
	/*!
	   \param cell A handler for the Cell to erase.
	   \brief Erases a Cell instance.
	 */
	void erase(const Cell & cell);

	//! Allocate space for storing Cell entities
	/*!
	   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions cells), then it is worth reserving space for this amount before starting to build the standard cells.
	   \param size Minimum capacity for the cell container.
	 */
	void reserve(Cell, uint32_t size);

	//! Size of Cell's System
	/*!
	   \brief Returns the number of Cells.
	   \return The number of Cells.
	 */
	uint32_t size(Cell) const;

	//! Capacity of the Cell's System
	/*!
	   \return The capacity of the Cell EntitySystem.
	 */
	uint32_t capacity(Cell) const;

	//! Find a cell
	/*!
	   \brief Using the mapping, return a cell handler by cell's name.
	   \param The cell name.
	   \return Return a cell handler by cell's name.
	 */
	Cell find(Cell, std::string cellName);

	//! Cells iterator
	/*!
	   \return Range iterator for the cells.
	 */
	ophidian::util::Range<StandardCells::CellsIterator> range(Cell) const;

	//! Cell name getter
	/*!
	   \brief Get the name of a given cell.
	   \param cell Cell entity to get the name.
	   \return Name of the cell
	 */
	std::string name(const Cell & cell) const;

	//! Pins of a Cell
	/*!
	   \brief Returns a Container Wrapper for the Pins of a Cell.
	   \param cell A handler for the Cell we want to get the Pins.
	   \return Container Wrapper for the Pins of a Cell.
	 */
	entity_system::Association<Cell, Pin>::Parts pins(const Cell& cell) const;

	//! Make Cell Property
	/*!
	   \brief Creates a Property for the Standard Cell's Entity System.
	   \tparam Value value type of the Property.
	   \return An Cell => \p Value Map.
	 */
	template <typename Value>
	entity_system::Property<Cell, Value> makeProperty(Cell)
	const {
		return entity_system::Property<Cell, Value>(mCells);
	}

//--------------------------- Pins -------------------------------//

	//! Add Pin
	/*!
	   \brief Adds a pin instance. A pin has a name and a direction associated to it.  If the pin already exist then just return the existing pin.
	   \param name Name of the pin, used to identify it.
	   \return A handler for the created/existing pin.
	 */
	Pin add(Pin, const std::string & name, PinDirection direction);

	//! Erase Pin
	/*!
	   \param pin A handler for the pin to erase.
	   \brief Erases a Pin instance.
	 */
	void erase(const Pin & pin);

	//! Allocate space for storing pin entities
	/*!
	   \brief Using this function, it is possible to avoid superfluous memory allocation: if you know that the netlist you want to build will be large (e.g. it will contain millions pins), then it is worth reserving space for this amount before starting to build the standard cells.
	   \param size Minimum capacity for the Pin container.
	 */
	void reserve(Pin, uint32_t size);

	//! Size of Pin's System
	/*!
	   \brief Returns the number of pins.
	   \return The number of pins.
	 */
	uint32_t size(Pin) const;

	//! Capacity of the Pin's System
	/*!
	   \return The capacity of the Pin EntitySystem.
	 */
	uint32_t capacity(Pin) const;

	//! Find a pin
	/*!
	   \brief Using the mapping, return a pin handler by pin's name.
	   \param The pin name.
	   \return Return a pin handler by pin's name.
	 */
	Pin find(Pin, std::string pinName);

    //! Find a pin
    /*!
       \brief Using the mapping, return a pin handler by pin's name.
       \param The pin name.
       \return Return a pin handler by pin's name.
     */
    Pin find(Pin, std::string pinName) const;

	//! Pin name getter
	/*!
	   \brief Get the name of a given pin.
	   \param pin Pin entity to get the name.
	   \return Name of the pin
	 */
	std::string name(const Pin & pin) const;

	//! Pin direction getter
	/*!
	   \brief Get the direction of a given pin. A direction can be NOT_ASSIGNED, INPUT, OUTPUT or INOUT.
	   \param pin Pin entity to get the direction.
	   \return Direction of the pin
	 */
    PinDirection direction(const Pin & pin) const;

	//! Pin owner getter
	/*!
	   \brief Get the owner of a given pin.
	   \param pin Pin entity to get the owner.
	   \return Owner of the pin
	 */
	Cell owner(const Pin & pin);

	//! Pins iterator
	/*!
	   \return Range iterator for the Pins.
	 */
	ophidian::util::Range<StandardCells::PinsIterator> range(Pin) const;

	//! Make Pin Property
	/*!
	   \brief Creates a Property for the Standard Cell's Entity System.
	   \tparam Value value type of the Property.
	   \return An Pin => \p Value Map.
	 */
	template <typename Value>
	entity_system::Property<Pin, Value> makeProperty(Pin)
	const {
		return entity_system::Property<Pin, Value>(mPins);
	}
//--------------------------- Association -------------------------------//

	//! Add Pin into Cell
	/*!
	   \brief Adds a Pin to a given Cell.
	   \param cell A handler for the Cell we want to add a Pin.
	   \param pin A handler for the Pin we want to add in \p cell.
	 */
	//Maybe rename to create_association or associate...
	void add(const Cell& cell, const Pin& pin);

private:
	//cells entity system and properties
	entity_system::EntitySystem<Cell> mCells;
	entity_system::Property<Cell, std::string> mCellNames;

	//pins entity system and properties
	entity_system::EntitySystem<Pin> mPins;
	entity_system::Property<Pin, std::string> mPinNames;
	entity_system::Property<Pin, PinDirection> mPinDirections;

	//composition and aggregation relations
	entity_system::Composition<Cell, Pin> mCellPins;

	//std_cell and pin mapping
	std::unordered_map<std::string, Cell> mName2Cell;
	std::unordered_map<std::string, Pin> mName2Pin;
};

} //namespace ophidian

} //namespace standard_cell

#endif // OPHIDIAN_STANDARD_CELL_STANDARD_CELLS_H
