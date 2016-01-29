/*
 * netlist.h
 *
 *  Created on: Jan 29, 2016
 *      Author: csguth
 */

#ifndef SRC_NETLIST_NETLIST_H_
#define SRC_NETLIST_NETLIST_H_

#include <standard_cells.h>
#include <entity.h>
#include "cells.h"
#include "pins.h"
#include "nets.h"

namespace openeda {
namespace netlist {

class netlist {

	standard_cell::standard_cells * m_std_cells;
	entity::system m_cells_system;
	entity::system m_pins_system;
	entity::system m_nets_system;

	cells m_cells;
	pins m_pins;
	nets m_nets;

	std::unordered_map<std::string, entity::entity> m_name2cell;
	std::unordered_map<std::string, entity::entity> m_name2pin;
	std::unordered_map<std::string, entity::entity> m_name2net;


public:
	netlist(standard_cell::standard_cells * std_cells);
	virtual ~netlist();

	std::size_t cell_count() const { return m_cells_system.size(); }

	entity::entity cell_insert(std::string name, std::string type);
	void cell_remove(entity::entity cell);

	std::string cell_name(entity::entity cell) const { return m_cells.name(cell); }
	std::vector<entity::entity> cell_pins(entity::entity cell) const { return m_cells.pins(cell); }

	entity::entity pin_insert(entity::entity cell, std::string name);

	entity::entity pin_owner(entity::entity pin) const { return m_pins.owner(pin); };
	std::size_t pin_count() const { return m_pins_system.size(); }
	entity::entity pin_net(entity::entity pin) const { return m_pins.net(pin); }

	entity::entity net_insert(std::string name);
	std::string net_name(entity::entity net) const { return m_nets.name(net); }
	std::vector<entity::entity> net_pins(entity::entity net) const { return m_nets.pins(net); }
	std::size_t net_count() const { return m_nets_system.size(); }

	void connect(entity::entity net, entity::entity pin);
	void disconnect(entity::entity pin);




};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_NETLIST_H_ */
