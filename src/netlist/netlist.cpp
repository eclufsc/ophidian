/*
 * netlist.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: csguth
 */

#include <netlist.h>
#include <algorithm>
#include <iostream>

namespace openeda {
namespace netlist {


netlist::netlist(standard_cell::standard_cells * std_cells) :
		m_std_cells(std_cells), m_cells(m_cells_system), m_pins(m_pins_system), m_nets(m_nets_system) {
}

netlist::~netlist() {
}


void netlist::register_cell_property(
		entity::property* property) {
	m_cells_system.register_property(property);
}

void netlist::register_pin_property(
		entity::property* property) {
	m_pins_system.register_property(property);
}

entity::entity netlist::cell_find(std::string name) {
	auto result = m_name2cell.find(name);
	if(result == m_name2cell.end())
		return entity::entity{};
	return result->second;
}

entity::entity netlist::cell_insert(std::string name, std::string type) {

	auto result = m_name2cell.find(name);
	if (result != m_name2cell.end())
		return result->second;
	entity::entity the_cell = m_cells_system.create();
	m_name2cell[name] = the_cell;
	m_cells.name(the_cell, name);
	m_cells.standard_cell(the_cell, m_std_cells->cell_create(type));
	return the_cell;
}

void netlist::cell_remove(entity::entity cell) {
	m_name2cell.erase(m_cells.name(cell));
	auto cell_pins = m_cells.pins(cell);
	for (auto pin : cell_pins)
		if (!(pin_net(pin) == entity::entity()))
			disconnect(pin);
	std::for_each(cell_pins.begin(), cell_pins.end(),
			std::bind(&entity::system::destroy, &m_pins_system,
					std::placeholders::_1));
	m_cells_system.destroy(cell);
}

entity::entity netlist::pin_insert(entity::entity cell, std::string name) {
	const std::string owner_name = m_cells.name(cell);
	const std::string pin_name =  owner_name + ":" + name;

	auto result = m_name2pin.find(pin_name);
	if (result != m_name2pin.end())
		return result->second;

	entity::entity the_pin = m_pins_system.create();
	m_name2pin[pin_name] = the_pin;
	m_pins.owner(the_pin, cell);
	m_pins.name(the_pin, name);

	entity::entity std_cell_pin = m_std_cells->pin_create(m_cells.standard_cell(cell), name);
	m_pins.standard_cell_pin(the_pin, std_cell_pin);
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

entity::entity netlist::PI_insert(std::string name) {
	auto result = m_name2pin.find(name);
	if (result != m_name2pin.end())
		return result->second;


	entity::entity the_pin = m_pins_system.create();
	m_name2pin[name] = the_pin;
	m_pins.standard_cell_pin(the_pin, m_std_cells->pad_create(name));
	m_PI_mapping.insert(entity2index_map::value_type(the_pin, m_PI.size()));
	m_PI.push_back(the_pin);
	return the_pin;
}

void netlist::net_remove(entity::entity net) {
	m_name2net.erase(m_nets.name(net));
	auto net_pins = m_nets.pins(net);
	for (auto pin : net_pins)
		if (!(pin_net(pin) == entity::entity()))
			disconnect(pin);
	m_nets_system.destroy(net);
}

void netlist::PI_remove(entity::entity PI) {
	std::size_t index = m_PI_mapping.left.at(PI);
	std::size_t last_index = m_PI.size() - 1;
	entity::entity last_entity = m_PI_mapping.right.at(last_index);
	m_PI[index] = last_entity;
	m_PI.pop_back();

	auto current_it = m_PI_mapping.left.find(PI);
	m_PI_mapping.left.erase(current_it);

	if (!(pin_net(PI) == entity::entity()))
		disconnect(PI);
	m_pins_system.destroy(PI);

	if (m_PI.size() > 1) {
		current_it = m_PI_mapping.left.find(last_entity);
		m_PI_mapping.left.erase(current_it);
		m_PI_mapping.insert(entity2index_map::value_type(last_entity, index));
	}
}

entity::entity netlist::PO_insert(std::string name) {
	auto result = m_name2pin.find(name);
	if (result != m_name2pin.end())
		return result->second;
	entity::entity the_pin = m_pins_system.create();
	m_name2pin[name] = the_pin;
	m_pins.standard_cell_pin(the_pin, m_std_cells->pad_create(name));
	m_PO_mapping.insert(entity2index_map::value_type(the_pin, m_PO.size()));
	m_PO.push_back(the_pin);
	return the_pin;
}

void netlist::PO_remove(entity::entity PO) {
	std::size_t index = m_PO_mapping.left.at(PO);
	std::size_t last_index = m_PO.size() - 1;
	entity::entity last_entity = m_PO_mapping.right.at(last_index);
	m_PO[index] = last_entity;
	m_PO.pop_back();

	auto current_it = m_PO_mapping.left.find(PO);
	m_PO_mapping.left.erase(current_it);

	if (!(pin_net(PO) == entity::entity()))
		disconnect(PO);
	m_pins_system.destroy(PO);

	if (m_PO.size() > 1) {
		current_it = m_PO_mapping.left.find(last_entity);
		m_PO_mapping.left.erase(current_it);
		m_PO_mapping.insert(entity2index_map::value_type(last_entity, index));
	}
}

bool netlist::cell_std_cell(entity::entity cell, std::string type) {
	auto new_std_cell = m_std_cells->cell_create(type);

	return cell_std_cell(cell, new_std_cell);
}

bool netlist::cell_std_cell(entity::entity cell, entity::entity std_cell) {
	auto current_std_cell = m_cells.standard_cell(cell);

	auto current_std_cell_pins = m_std_cells->cell_pins(current_std_cell);
	auto new_std_cell_pins = m_std_cells->cell_pins(std_cell);
	if (new_std_cell_pins.size() != current_std_cell_pins.size()) {
		return false;
	}
	m_cells.standard_cell(cell, std_cell);
	for (auto pin : m_cells.pins(cell)) {
		entity::entity std_cell_pin = m_std_cells->pin_create(std_cell, m_pins.name(pin));
		m_pins.standard_cell_pin(pin, std_cell_pin);
	}
	return true;
}
} /* namespace netlist */
} /* namespace openeda */


