/*
 * netlist.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: csguth
 */

#include <netlist.h>
#include <algorithm>

namespace openeda {
namespace netlist {

netlist::netlist(standard_cell::standard_cells * std_cells) :
		m_std_cells(std_cells), m_cells(&m_cells_system), m_pins(
				&m_pins_system), m_nets(&m_nets_system) {
}

netlist::~netlist() {
}

entity::entity netlist::cell_insert(std::string name, std::string type) {

	auto result = m_name2cell.find(name);
	if (result != m_name2cell.end())
		return result->second;
	entity::entity the_cell = m_cells_system.create();
	m_name2cell[name] = the_cell;
	m_cells.name(the_cell, name);
	m_cells.standard_cell(the_cell, m_std_cells->create(type));
	return the_cell;
}

void netlist::cell_remove(entity::entity cell) {
	m_name2cell.erase(m_cells.name(cell));
	auto cell_pins = m_cells.pins(cell);
	for(auto pin : cell_pins)
		if( !(pin_net(pin) == entity::entity()) )
			disconnect(pin);
	std::for_each(cell_pins.begin(), cell_pins.end(),
			std::bind(&entity::system::destroy, &m_pins_system,
					std::placeholders::_1));
	m_cells_system.destroy(cell);
}

entity::entity netlist::pin_insert(entity::entity cell, std::string name) {
	auto result = m_name2pin.find(name);
	if (result != m_name2pin.end())
		return result->second;
	entity::entity the_pin = m_pins_system.create();
	m_name2pin[name] = the_pin;
	m_pins.owner(the_pin, cell);
	m_pins.name(the_pin, name);
	m_cells.insert_pin(cell, the_pin);
	return the_pin;
}

entity::entity netlist::net_insert(std::string name) {
	auto result = m_name2net.find(name);
	if (result != m_name2net.end())
		return result->second;
	entity::entity the_net = m_nets_system.create();
	m_name2net[name] = the_net;
	m_nets.name(the_net, name);
	return the_net;
}

void netlist::connect(entity::entity net, entity::entity pin) {
	auto current_net_of_pin = m_pins.net(pin);
	if (current_net_of_pin == net)
		return;
	if (!(current_net_of_pin == entity::entity())) // disconnected
		throw std::runtime_error("cannot connect an already connected pin!!");
	m_nets.connect(net, pin);
	m_pins.net(pin, net);
}

void netlist::disconnect(entity::entity pin) {
	m_nets.disconnect(pin_net(pin), pin);
	m_pins.net(pin, entity::entity());
}

} /* namespace netlist */
} /* namespace openeda */

