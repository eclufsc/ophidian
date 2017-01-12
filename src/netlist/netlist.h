/*
 * Copyright 2016 Ophidian
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

#ifndef SRC_NETLIST_NETLIST_H_
#define SRC_NETLIST_NETLIST_H_

#include "../standard_cell/standard_cells.h"
#include "../entity_system/entity_system.h"
#include "../geometry/geometry.h"
#include <boost/bimap.hpp>
#include "cells.h"
#include "pins.h"
#include "nets.h"

#include  <iostream>
#include <unordered_map>

namespace ophidian {
	/// Namespace describing netlist entities and basic netlist interface.
namespace netlist {
	/// Netlist class.
	/**
	 * This class provides the basic netlist interface, such as cells, pins and nets manipulation.
	 */
class netlist {

	standard_cell::standard_cells * m_std_cells;

	std::string m_module_name;

    entity_system::entity_system m_cells_system;
    entity_system::entity_system m_pins_system;
    entity_system::entity_system m_nets_system;

	cells m_cells;
	pins m_pins;
	nets m_nets;

    using entity2index_map = typename boost::bimap< entity_system::entity, std::size_t >;
	entity2index_map m_PI_mapping;
	entity2index_map m_PO_mapping;

    std::vector<entity_system::entity> m_PI;
    std::vector<entity_system::entity> m_PO;

    std::unordered_map<std::string, entity_system::entity> m_name2cell;
    std::unordered_map<std::string, entity_system::entity> m_name2pin;
    std::unordered_map<std::string, entity_system::entity> m_name2net;

public:

    void cell_preallocate(std::size_t qnt);
    void pin_preallocate(std::size_t qnt);
    void net_preallocate(std::size_t qnt);

    void set_pin_position(entity_system::entity pin, const geometry::point<double> & position);

    geometry::point<double> get_pin_position(entity_system::entity pin);

	/// Constructor.
	/**
	 * Netlist constructor. Creates the entity systems for cells, pins and nets.
	 * \param std_cells Standard cells object.
	 */
	netlist(standard_cell::standard_cells * std_cells);

	virtual ~netlist();

	/// Registers cell property.
	/**
	 * Registers a property to the cells entity system.
	 * \param property Property to be registered.
	 */
    void register_cell_property(entity_system::property * property);
	/// Registers pin property.
	/**
	 * Registers a property to the pins entity system.
	 * \param property Property to be registered.
	 */
    void register_pin_property(entity_system::property * property);
	/// Registers net property.
	/**
	 * Registers a property to the nets entity system.
	 * \param property Property to be registered.
	 */
    void register_net_property(entity_system::property * property);

	void module_name(std::string name) {
		m_module_name = name;
	}
	std::string module_name() const {
		return m_module_name;
	}

	/// Standard cells getter.
	/**
	 * Returns the standard cells object.
	 * \return Constant reference to the standard cells object.
	 */
    const standard_cell::standard_cells & std_cells() const {
        return *m_std_cells;
    }

	// cell
	/// Finds cell.
	/**
	 * Finds a cell by its name.
	 * \param name Cell name.
	 * \return Entity representing the found cell.
	 */
    entity_system::entity cell_find(std::string name) const;
	/// Inserts a new cell.
	/**
     * Inserts a new cell in the netlist. A cell has a name and type associated to it.
     * \param name Name of the cell, used to identify it.
     * \param type Standard cell type of the cell.
     * \return The created cell.
     */
    entity_system::entity cell_insert(std::string name, std::string type);
	/// Removes a cell.
	/**
	 * Removes an existing cell from the netlist.
	 * \param cell Cell to be removed.
	 */
    void cell_remove(entity_system::entity cell);

	/// Returns the number of cells.
	/**
	 * Returns the number of cells created in the cells system.
	 * \return Number of cells.
	 */
	std::size_t cell_count() const {
		return m_cells_system.size();
	}
	/// Cell name getter.
	/**
	 * Returns the name of a cell.
	 * \param cell Cell to get the name.
	 * \return Name of the cell.
	 */
    std::string cell_name(entity_system::entity cell) const {
		return m_cells.name(cell);
	}
	/// Cell pins getter.
	/**
	 * Returns the pins of a cell.
	 * \param cell Cell to get the pins.
	 * \return Vector containing all pins of the cell.
	 */
    const std::vector<entity_system::entity> & cell_pins(entity_system::entity cell) const {
		return m_cells.pins(cell);
	}
	/// Cell type getter.
	/**
	 * Returns the standard cell type of a cell.
	 * \param cell Cell to get the type.
	 * \return Entity representing the standard cell type of the cell.
	 */
    entity_system::entity cell_std_cell(entity_system::entity cell) const {
		return m_cells.standard_cell(cell);
	}
	/// Cell type setter.
	/**
	 * Sets the standard cell type of a cell. The number of pins of the current cell type and the new one must match.
	 * \param cell Cell to set the type.
	 * \param type Name of the type to set for the cell.
	 * \return bool variable describing if it was possible to set the type of the cell.
	 */
    bool cell_std_cell(entity_system::entity cell, std::string type);
	/// Cell type setter.
	/**
	 * Sets the standard cell type of a cell. The number of pins of the current cell type and the new one must match.
	 * \param cell Cell to set the type.
	 * \param type Entity of the type to set for the cell.
	 * \return bool variable describing if it was possible to set the type of the cell.
	 */
    bool cell_std_cell(entity_system::entity cell, entity_system::entity std_cell);
	/// Cell system getter.
	/**
	 * Returns the cells entity system.
	 * \return Constant reference to the cells entity system.
	 */
    const entity_system::entity_system & cell_system() const {
		return m_cells_system;
	}
	/// Cell properties getter.
	/**
	 * Returns the cells properties object.
	 * \return Constant reference to the object containing the cells properties.
	 */
	const cells & cells_properties() const {
		return m_cells;
	}

	// pin
	/// Inserts a new pin.
	/**
     * Inserts a new pin in the netlist. A pin has a name and owner associated to it.
     * \param cell Cell owner of the pin.
     * \param name Name of the pin, used to identify it.
     * \return The created pin.
     */
    entity_system::entity pin_insert(entity_system::entity cell, std::string name);
	/// Returns the number of pins.
	/**
	 * Returns the number of pins created in the pins system.
	 * \return Number of pins.
	 */
	std::size_t pin_count() const {
		return m_pins_system.size();
	}
	/// Pin name getter.
	/**
	 * Returns the name of a pin.
	 * \param pin Pin to get the name.
	 * \return Name of the pin.
	 */
    std::string pin_name(entity_system::entity pin) const {
		auto owner = m_pins.owner(pin);
		std::string the_name;
        if (!(owner == entity_system::invalid_entity))
		{
			the_name = m_cells.name(owner) + ":";
			std::string std_cell_pin_name = m_std_cells->pin_name(m_pins.standard_cell_pin(pin));
			std_cell_pin_name = std_cell_pin_name.substr(std_cell_pin_name.find_last_of(':')+1);
			the_name += std_cell_pin_name;
		} else
			the_name = m_std_cells->pin_name(m_pins.standard_cell_pin(pin));
		return the_name;
	}
	/// Pin owner getter.
	/**
	 * Returns the owner of a pin.
	 * \param pin Pin to get the owner.
	 * \return Entity describing the cell owning the pin.
	 */
    entity_system::entity pin_owner(entity_system::entity pin) const {
		return m_pins.owner(pin);
	}
	/// Pin net getter.
	/**
	 * Returns the net of a pin.
	 * \param pin Pin to get the net.
	 * \return Entity describing the net of the pin.
	 */
    entity_system::entity pin_net(entity_system::entity pin) const {
		return m_pins.net(pin);
	}
	/// Pin type getter.
	/**
	 * Returns the standard cell type of a pin.
	 * \param pin Pin to get the type.
	 * \return Entity describing the standard cell type of a pin.
	 */
    entity_system::entity pin_std_cell(entity_system::entity pin) const {
		return m_pins.standard_cell_pin(pin);
	}
	/// Finds pin.
	/**
	 * Finds a pin by its name.
	 * \param name Pin name.
	 * \return Entity representing the found pin.
	 */
    entity_system::entity pin_by_name(std::string name) const {
		return m_name2pin.at(name);
	}
	/// Pin system getter.
	/**
	 * Returns the pins entity system.
	 * \return Constant reference to the pins entity system.
	 */
    const entity_system::entity_system & pin_system() const {
		return m_pins_system;
	}
	/// Pin properties getter.
	/**
	 * Returns the pins properties object.
	 * \return Constant reference to the object containing the pins properties.
	 */
	const pins & pins_properties() const {
		return m_pins;
	}

	// net
	/// Inserts a new net.
	/**
     * Inserts a new net in the netlist. A net has a name associated to it.
     * \param name Name of the net, used to identify it.
     * \return The created net.
     */
    entity_system::entity net_insert(std::string name);
    /// Inserts a new net.
    /**
     * Inserts a new net in the netlist. A net has a name associated to it.
     * \param name Name of the net, used to identify it.
     * \param pin_count Number of pins the net will connect
     * \return The created net.
     */
    entity_system::entity net_insert(std::string name, std::size_t pin_count);
	/// Removes a net.
	/**
	 * Removes an existing net from the netlist.
	 * \param net Net to be removed.
	 */
    void net_remove(entity_system::entity net);
	/// Returns the number of nets.
	/**
	 * Returns the number of nets created in the pins system.
	 * \return Number of pins.
	 */
	std::size_t net_count() const {
		return m_nets_system.size();
	}
	/// Net name getter.
	/**
	 * Returns the name of a net.
	 * \param net Net to get the name.
	 * \return Name of the net.
	 */
    std::string net_name(entity_system::entity net) const {
		return m_nets.name(net);
	}
	/// Net pins getter.
	/**
	 * Returns the pins of a net.
	 * \param net Net to get the pins.
	 * \return Vector containing all pins of the net.
	 */
    const std::vector<entity_system::entity> & net_pins(entity_system::entity net) const {
		return m_nets.pins(net);
	}

	/// Net names iterator.
	/**
	 * Returns the beginning and end iterators of the net names property.
	 * \return Pair of iterators pointing to the begin and end of the net names property.
	 */
	std::pair<std::vector<std::string>::const_iterator,
	std::vector<std::string>::const_iterator> net_names() const {
		return m_nets.names();
	}
	/// Net system getter.
	/**
	 * Returns the nets entity system.
	 * \return Constant reference to the nets entity system.
	 */
    const entity_system::entity_system & net_system() const {
		return m_nets_system;
	}
	/// Finds net.
	/**
	 * Finds a net by its name.
	 * \param name Net name.
	 * \return Entity representing the found net.
	 */
    entity_system::entity net_by_name(std::string name) const {
		return m_name2net.at(name);
	}
	/// Net properties getter.
	/**
	 * Returns the nets properties object.
	 * \return Constant reference to the object containing the nets properties.
	 */
	const nets & nets_properties() const {
		return m_nets;
	}

	/// Connects a pin to a net.
	/**
	 * Connects a pin to a net. This method changes the pins property of the net to contain the new pin and the net property of the pin.
	 * \param net Net to be connnected.
	 * \param pin Pin to be connected.
	 */
    void connect(entity_system::entity net, entity_system::entity pin);
	/// Disconnects a pin from its net.
	/**
	 * Disconnects a pin from its net. This method removes the pin from the net pins property and leaves the pin without a net.
	 * \param pin Pin to be disconnected.
	 */
    void disconnect(entity_system::entity pin);

	// PI
	/// Inserts a new primary input.
	/**
     * Inserts a new primary input in the netlist. A primary input is a pin without an owner.
     * \param name Name of the primary input, used to identify it.
     * \return The created primary input.
     */
    entity_system::entity PI_insert(std::string name);
	/// Removes a primary input.
	/**
	 * Removes an existing primary input from the netlist.
	 * \param PI Primary input to be removed.
	 */
    void PI_remove(entity_system::entity PI);
	/// Returns the number of primary inputs.
	/**
	 * Returns the number of primary inputs created in the netlist.
	 * \return Number of primary inputs.
	 */
	std::size_t PI_count() const {
		return m_PI.size();
	}
	/// Primary input begin iterator.
	/**
	 * Returns an iterator pointing to the beginning of the primary inputs in the netlist.
	 * \return Constant iterator pointing to the beginning of the primary inputs.
	 */
    std::vector<entity_system::entity>::const_iterator PI_begin() const {
		return m_PI.begin();
	}
	/// Primary input end iterator.
	/**
	 * Returns an iterator pointing to the end of the primary inputs in the netlist.
	 * \return Constant iterator pointing to the end of the primary inputs.
	 */
    std::vector<entity_system::entity>::const_iterator PI_end() const {
		return m_PI.end();
	}

	// PO
	/// Inserts a new primary output.
	/**
     * Inserts a new primary output in the netlist. A primary input is a pin without an owner.
     * \param name Name of the primary output, used to identify it.
     * \return The created primary output.
     */
    entity_system::entity PO_insert(std::string name);
	/// Removes a primary output.
	/**
	 * Removes an existing primary output from the netlist.
	 * \param PO Primary output to be removed.
	 */
    void PO_remove(entity_system::entity PO);
	/// Returns the number of primary outputs.
	/**
	 * Returns the number of primary outputs created in the netlist.
	 * \return Number of primary outputs.
	 */
	std::size_t PO_count() const {
		return m_PO.size();
	}
	/// Primary output begin iterator.
	/**
	 * Returns an iterator pointing to the beginning of the primary outputs in the netlist.
	 * \return Constant iterator pointing to the beginning of the primary outputs.
	 */
    std::vector<entity_system::entity>::const_iterator PO_begin() const {
		return m_PO.begin();
	}
	/// Primary output end iterator.
	/**
	 * Returns an iterator pointing to the end of the primary outputs in the netlist.
	 * \return Constant iterator pointing to the end of the primary outputs.
	 */
    std::vector<entity_system::entity>::const_iterator PO_end() const {
		return m_PO.end();
	}

};

} /* namespace netlist */
} /* namespace ophidian */

#endif /* SRC_NETLIST_NETLIST_H_ */
