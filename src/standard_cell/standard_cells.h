/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_STANDARD_CELL_STANDARD_CELLS_H_
#define SRC_STANDARD_CELL_STANDARD_CELLS_H_

#include "../entity/entity.h"
#include "../standard_cell/cells.h"
#include "../standard_cell/pins.h"

namespace ophidian {
	/// Namespace describing standard cell entities and basic standard cell interface.
namespace standard_cell {

/// Standard cell class.
		/**
         * This class provides the basic standard cell interface, such as creation of cell and pin types.
         */
class standard_cells {

	entity::system m_cell_system;
	entity::system m_pin_system;

	cells m_cells;
	pins m_pins;

	std::unordered_map<std::string, entity::entity> m_name2cell;
	std::unordered_map<std::string, entity::entity> m_name2pin;
public:
	/// Constructor.
	/**
	 * Default constructor of the standard cell class. Initializes an empty system for cells and pins.
	 */
	standard_cells();
	virtual ~standard_cells();

	/// Registers cell property.
	/**
	 * Registers a property to the cells entity system.
	 * \param property Property to be registered.
	 */
	void register_cell_property(entity::property* property);
	/// Registers pin property.
	/**
	 * Registers a property to the pins entity system.
	 * \param property Property to be registered.
	 */
	void register_pin_property(entity::property* property);

	/// Cell names getter.
	/**
	 * Returns the names of all cells.
	 * \return Constant reference to the cell names property.
	 */
	const entity::vector_property<std::string> & cell_names() const {
		return m_cells.names();
	}
	/// Creates a cell.
	/**
	 * Creates a new cell in the standard cell library.
	 * \param name Name of the new cell.
	 * \return Entity of the created cell.
	 */
	entity::entity cell_create(std::string name);
	/// Cell name getter.
	/**
	 * Returns the name of a cell.
	 * \param cell Cell entity to get the name.
	 * \return Name of the cell.
	 */
	std::string cell_name(entity::entity cell) const {
		return m_cells.name(cell);
	}
	/// Cell pins getter.
	/**
	 * Returns the pins of a cell.
	 * \param cell Cell entity to get the pins.
	 * \return Constant reference to a vector with all pins of that cell.
	 */
	const std::vector<entity::entity> & cell_pins(entity::entity cell) const {
		return m_cells.pins(cell);
	}
	/// Returns the number of cells.
	/**
     * Returns the number of cells created in the cells system.
     * \return Number of cells.
     */
	std::size_t cell_count() const {
		return m_cell_system.size();
	}
	/// Cell system getter.
	/**
	 * Returns the cell system.
	 * \return Constant reference to the cell system.
	 */
	const entity::system & cell_system() const {
		return m_cell_system;
	}
	/// Cell sequential attribute setter.
	/**
	 * Sets if a cell is a sequential cell.
	 * \param cell Cell entity to be set.
	 * \param sequential bool variable describing if the cell is sequential or not.
	 */
    void  cell_sequential(entity::entity cell, bool sequential);
	/// Cell sequential attribute getter.
	/**
	 * Gets if a cell is a sequential cell.
	 * \param cell Cell entity to get the attribute.
	 * \return bool variable describing if the cell is sequential or not.
	 */
    bool cell_sequential(entity::entity cell) const {
        return m_cells.sequential(cell);
    }

	/// Creates a pin.
	/**
	 * Creates a new pin in the standard cell library.
	 * \param cell Cell entity owner of the new pin.
	 * \param name Name of the new pin.
	 * \return Entity of the created pin.
	 */
	entity::entity pin_create(entity::entity cell, std::string name);
	/// Pin owner getter.
	/**
	 * Returns the owner of a pin.
	 * \param pin Pin entity to get the owner.
	 * \return Owner of the pin.
	 */
	entity::entity pin_owner(entity::entity pin) const
	{
		return m_pins.owner(pin);
	}
	/// Pin name getter.
	/**
	 * Returns the name of a pin.
	 * \param pin Pin entity to get the name.
	 * \return Name of the pin.
	 */
	std::string pin_name(entity::entity pin) const {
		std::string the_name;
		entity::entity owner{m_pins.owner(pin)};
		if(!(owner == entity::entity{}))
			the_name = m_cells.name(owner) + ":";
		the_name += m_pins.name(pin);
		return the_name;
	}
	/// Returns the number of pins.
	/**
     * Returns the number of pins created in the pins system.
     * \return Number of pins.
     */
	std::size_t pin_count() const {
		return m_pin_system.size();
	}
	/// Pin system getter.
	/**
	 * Returns the pin system.
	 * \return Constant reference to the pin system.
	 */
	const entity::system & pin_system() const {
		return m_pin_system;
	}
	/// Pin direction getter.
	/**
	 * Returns the direction of a pin. Possible direction values are NOT_ASSIGNED, INPUT and OUTPUT.
	 * \param pin Pin entity to get the name.
	 * \return Direction of the pin.
	 */
	pin_directions pin_direction(entity::entity pin) const {
		return m_pins.direction(pin);
	}
	/// Pin clock input attribute getter.
	/**
	 * Gets if a pin is a clock input.
	 * \param pin Pin entity to get the attribute.
	 * \return bool variable describing if the pin is a clock input or not.
	 */
    bool pin_clock_input(entity::entity pin) const {
        return m_pins.clock_input(pin);
    }
	/// Pin clock input attribute setter.
	/**
	 * Sets if a pin is a clock input.
	 * \param pin Pin entity to be set.
	 * \param clock_input bool variable describing if the pin is a clock input or not.
	 */
    void pin_clock_input(entity::entity pin, bool clock_input);
	/// Pin direction setter.
	/**
	 * Sets the direction of a pin. Possible direction values are NOT_ASSIGNED, INPUT and OUTPUT.
	 * \param pin Pin entity to sets the direction.
	 * \param Direction of the pin.
	 */
	void pin_direction(entity::entity pin, pin_directions direction);

	/// Creates a pad.
	/**
	 * Creates a new pad in the standard cell library. A pad is a special pin without owner.
	 * \param name Name of the new pad.
	 * \return Entity of the created pad.
	 */
	entity::entity pad_create(std::string name);



};

} /* namespace standard_cell */
} /* namespace ophidian */

#endif /* SRC_STANDARD_CELL_STANDARD_CELLS_H_ */
