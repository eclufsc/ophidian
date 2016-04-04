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

#ifndef SRC_NETLIST_NETLIST_H_
#define SRC_NETLIST_NETLIST_H_

#include "../standard_cell/standard_cells.h"
#include "../entity/entity.h"
#include <boost/bimap.hpp>
#include "cells.h"
#include "pins.h"
#include "nets.h"

#include  <iostream>

namespace ophidian {
namespace netlist {

class netlist {

	standard_cell::standard_cells * m_std_cells;

	std::string m_module_name;

	entity::system m_cells_system;
	entity::system m_pins_system;
	entity::system m_nets_system;

	cells m_cells;
	pins m_pins;
	nets m_nets;

	using entity2index_map = typename boost::bimap< entity::entity, std::size_t >;
	entity2index_map m_PI_mapping;
	entity2index_map m_PO_mapping;

	std::vector<entity::entity> m_PI;
	std::vector<entity::entity> m_PO;

	std::unordered_map<std::string, entity::entity> m_name2cell;
	std::unordered_map<std::string, entity::entity> m_name2pin;
	std::unordered_map<std::string, entity::entity> m_name2net;

public:
	netlist(standard_cell::standard_cells * std_cells);
	virtual ~netlist();

	void register_cell_property(entity::property * property);
	void register_pin_property(entity::property * property);
	void register_net_property(entity::property * property);

	void module_name(std::string name) {
		m_module_name = name;
	}
	std::string module_name() const {
		return m_module_name;
	}

	// cell
    entity::entity cell_find(std::string name) const;
	entity::entity cell_insert(std::string name, std::string type);
	void cell_remove(entity::entity cell);

	std::size_t cell_count() const {
		return m_cells_system.size();
	}
	std::string cell_name(entity::entity cell) const {
		return m_cells.name(cell);
	}
	std::vector<entity::entity> cell_pins(entity::entity cell) const {
		return m_cells.pins(cell);
	}
	entity::entity cell_std_cell(entity::entity cell) const {
		return m_cells.standard_cell(cell);
	}
	bool cell_std_cell(entity::entity cell, std::string type);
	bool cell_std_cell(entity::entity cell, entity::entity std_cell);
	const entity::system & cell_system() const {
		return m_cells_system;
	}

	const cells & cells_properties() const {
		return m_cells;
	}

	// pin
	entity::entity pin_insert(entity::entity cell, std::string name);
	std::size_t pin_count() const {
		return m_pins_system.size();
	}
	std::string pin_name(entity::entity pin) const {
		auto owner = m_pins.owner(pin);
		std::string the_name;
		if (!(owner == entity::entity { }))
		{
			the_name = m_cells.name(owner) + ":";
			std::string std_cell_pin_name = m_std_cells->pin_name(m_pins.standard_cell_pin(pin));
			std_cell_pin_name = std_cell_pin_name.substr(std_cell_pin_name.find_last_of(':')+1);
			the_name += std_cell_pin_name;
		} else
			the_name = m_std_cells->pin_name(m_pins.standard_cell_pin(pin));
		return the_name;
	}
	entity::entity pin_owner(entity::entity pin) const {
		return m_pins.owner(pin);
	}

	entity::entity pin_net(entity::entity pin) const {
		return m_pins.net(pin);
	}
	entity::entity pin_std_cell(entity::entity pin) const {
		return m_pins.standard_cell_pin(pin);
	}
	entity::entity pin_by_name(std::string name) const {
		return m_name2pin.at(name);
	}
	const entity::system & pin_system() const {
		return m_pins_system;
	}

	const pins & pins_properties() const {
		return m_pins;
	}

	// net
	entity::entity net_insert(std::string name);
	void net_remove(entity::entity net);
	std::size_t net_count() const {
		return m_nets_system.size();
	}
	std::string net_name(entity::entity net) const {
		return m_nets.name(net);
	}
	std::vector<entity::entity> net_pins(entity::entity net) const {
		return m_nets.pins(net);
	}
	std::pair<std::vector<std::string>::const_iterator,
	std::vector<std::string>::const_iterator> net_names() const {
		return m_nets.names();
	}
	const entity::system & net_system() const {
		return m_nets_system;
	}
	entity::entity net_by_name(std::string name) const {
		return m_name2net.at(name);
	}

	const nets & nets_properties() const {
		return m_nets;
	}

	void connect(entity::entity net, entity::entity pin);
	void disconnect(entity::entity pin);

	// PI
	entity::entity PI_insert(std::string name);
	void PI_remove(entity::entity PI);
	std::size_t PI_count() const {
		return m_PI.size();
	}
	std::vector<entity::entity>::const_iterator PI_begin() const {
		return m_PI.begin();
	}
	std::vector<entity::entity>::const_iterator PI_end() const {
		return m_PI.end();
	}

	// PO
	entity::entity PO_insert(std::string name);
	void PO_remove(entity::entity PI);
	std::size_t PO_count() const {
		return m_PO.size();
	}
	std::vector<entity::entity>::const_iterator PO_begin() const {
		return m_PO.begin();
	}
	std::vector<entity::entity>::const_iterator PO_end() const {
		return m_PO.end();
	}

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_NETLIST_H_ */
