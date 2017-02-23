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

#include <netlist.h>
#include <algorithm>
#include <iostream>

namespace ophidian {
namespace netlist {

void netlist::pin_preallocate(std::size_t qnt)
{
    m_pins_system.preallocate(qnt);
}

void netlist::net_preallocate(std::size_t qnt)
{
    m_pins_system.preallocate(qnt);
}

void netlist::cell_preallocate(std::size_t qnt)
{
    m_pins_system.preallocate(qnt);
}

netlist::netlist(standard_cell::standard_cells * std_cells) :
		m_std_cells(std_cells), m_cells(m_cells_system), m_pins(m_pins_system), m_nets(m_nets_system) {
}

netlist::~netlist() {
}

void netlist::set_pin_position(entity_system::entity pin, const geometry::point<double> & position){
    m_pins.set_position(pin, position);
}

void netlist::set_pin_attributes(const entity_system::entity pin,
                                 const std::string pin_name,
                                 const entity_system::entity pin_owner,
                                 const entity_system::entity net_pin,
                                 const entity_system::entity std_cell,
                                 geometry::point<double> pin_position){
    m_pins.name(pin, pin_name);
    m_pins.owner(pin, pin_owner);
    m_pins.net(pin, net_pin);
    m_pins.standard_cell_pin(pin, std_cell);
    m_pins.set_position(pin, pin_position);
}

geometry::point<double> netlist::get_pin_position(entity_system::entity pin){
    return m_pins.pin_position(pin);
}

void netlist::register_cell_property(entity_system::property* property) {
	m_cells_system.register_property(property);
}

void netlist::register_pin_property(entity_system::property* property) {
	m_pins_system.register_property(property);
}

void netlist::register_net_property(entity_system::property* property) {
	m_nets_system.register_property(property);
}

entity_system::entity netlist::cell_find(std::string name) const {
	auto result = m_name2cell.find(name);
	if (result == m_name2cell.end())
        return entity_system::invalid_entity;
	return result->second;
}

entity_system::entity netlist::cell_insert(std::string name, std::string type) {

	auto result = m_name2cell.find(name);
	if (result != m_name2cell.end())
		return result->second;
    entity_system::entity the_cell = m_cells_system.create();
	m_name2cell[name] = the_cell;
	m_cells.name(the_cell, name);
    auto std_cell = m_std_cells->cell_create(type);
    m_cells.standard_cell(the_cell, std_cell);
    m_cells.pins_preallocate(the_cell, m_std_cells->cell_pins(std_cell).size());
    return the_cell;
}

void netlist::cell_remove(entity_system::entity cell) {
	m_name2cell.erase(m_cells.name(cell));
	auto cell_pins = m_cells.pins(cell);
	for (auto pin : cell_pins)
        if (!(pin_net(pin) == entity_system::invalid_entity))
			disconnect(pin);
    std::for_each(cell_pins.begin(), cell_pins.end(), std::bind(&entity_system::entity_system::destroy, &m_pins_system, std::placeholders::_1));
	m_cells_system.destroy(cell);
}

entity_system::entity netlist::pin_insert(entity_system::entity cell, std::string name) {
	const std::string owner_name = m_cells.name(cell);
	const std::string pin_name = owner_name + ":" + name;

	auto result = m_name2pin.find(pin_name);
	if (result != m_name2pin.end())
		return result->second;

    entity_system::entity the_pin = m_pins_system.create();
	m_name2pin[pin_name] = the_pin;
	m_pins.owner(the_pin, cell);
	m_pins.name(the_pin, name);

    entity_system::entity std_cell_pin = m_std_cells->pin_create(m_cells.standard_cell(cell), name);
	m_pins.standard_cell_pin(the_pin, std_cell_pin);
	m_cells.insert_pin(cell, the_pin);
	return the_pin;
}

entity_system::entity netlist::net_insert(std::string name) {
	auto result = m_name2net.find(name);
	if (result != m_name2net.end())
		return result->second;
    entity_system::entity the_net = m_nets_system.create();
	m_name2net[name] = the_net;
	m_nets.name(the_net, name);
    return the_net;
}

entity_system::entity netlist::net_insert(std::string name, std::size_t pin_count)
{
    auto the_net = net_insert(name);
    m_nets.preallocate_pins(the_net, pin_count);
    return the_net;

}

void netlist::connect(entity_system::entity net, entity_system::entity pin) {
	auto current_net_of_pin = m_pins.net(pin);
	if (current_net_of_pin == net)
		return;
    if (!(current_net_of_pin == entity_system::entity())) // disconnected
		throw std::runtime_error("cannot connect an already connected pin!!");
	m_nets.connect(net, pin);
	m_pins.net(pin, net);
}

void netlist::disconnect(entity_system::entity pin) {
	m_nets.disconnect(pin_net(pin), pin);
    m_pins.net(pin, entity_system::entity());
}

entity_system::entity netlist::PI_insert(std::string name) {
	auto result = m_name2pin.find(name);
	if (result != m_name2pin.end())
		return result->second;

    entity_system::entity the_pin = m_pins_system.create();
	m_name2pin[name] = the_pin;
	m_pins.standard_cell_pin(the_pin, m_std_cells->pad_create(name));
	m_PI_mapping.insert(entity2index_map::value_type(the_pin, m_PI.size()));
	m_PI.push_back(the_pin);
	return the_pin;
}

void netlist::net_remove(entity_system::entity net) {
	m_name2net.erase(m_nets.name(net));
	auto net_pins = m_nets.pins(net);
	for (auto pin : net_pins)
        if (!(pin_net(pin) == entity_system::invalid_entity))
			disconnect(pin);
	m_nets_system.destroy(net);
}

void netlist::PI_remove(entity_system::entity PI) {
	std::size_t index = m_PI_mapping.left.at(PI);
	std::size_t last_index = m_PI.size() - 1;
    entity_system::entity last_entity = m_PI_mapping.right.at(last_index);
	m_PI[index] = last_entity;
	m_PI.pop_back();

	auto current_it = m_PI_mapping.left.find(PI);
	m_PI_mapping.left.erase(current_it);

    if (!(pin_net(PI) == entity_system::invalid_entity))
		disconnect(PI);
	m_pins_system.destroy(PI);

	if (m_PI.size() > 1) {
		current_it = m_PI_mapping.left.find(last_entity);
		m_PI_mapping.left.erase(current_it);
		m_PI_mapping.insert(entity2index_map::value_type(last_entity, index));
	}
}

entity_system::entity netlist::PO_insert(std::string name) {
	auto result = m_name2pin.find(name);
	if (result != m_name2pin.end())
		return result->second;
    entity_system::entity the_pin = m_pins_system.create();
	m_name2pin[name] = the_pin;
	m_pins.standard_cell_pin(the_pin, m_std_cells->pad_create(name));
	m_PO_mapping.insert(entity2index_map::value_type(the_pin, m_PO.size()));
	m_PO.push_back(the_pin);
	return the_pin;
}

void netlist::PO_remove(entity_system::entity PO) {
	std::size_t index = m_PO_mapping.left.at(PO);
	std::size_t last_index = m_PO.size() - 1;
    entity_system::entity last_entity = m_PO_mapping.right.at(last_index);
	m_PO[index] = last_entity;
	m_PO.pop_back();

	auto current_it = m_PO_mapping.left.find(PO);
	m_PO_mapping.left.erase(current_it);

    if (!(pin_net(PO) == entity_system::invalid_entity))
		disconnect(PO);
	m_pins_system.destroy(PO);

	if (m_PO.size() > 1) {
		current_it = m_PO_mapping.left.find(last_entity);
		m_PO_mapping.left.erase(current_it);
		m_PO_mapping.insert(entity2index_map::value_type(last_entity, index));
	}
}

bool netlist::cell_std_cell(entity_system::entity cell, std::string type) {
	auto new_std_cell = m_std_cells->cell_create(type);

	return cell_std_cell(cell, new_std_cell);
}

bool netlist::cell_std_cell(entity_system::entity cell, entity_system::entity std_cell) {
	auto current_std_cell = m_cells.standard_cell(cell);

	auto current_std_cell_pins = m_std_cells->cell_pins(current_std_cell);
	auto new_std_cell_pins = m_std_cells->cell_pins(std_cell);
	if (new_std_cell_pins.size() != current_std_cell_pins.size()) {
		return false;
	}
	m_cells.standard_cell(cell, std_cell);
	for (auto pin : m_cells.pins(cell)) {
        entity_system::entity std_cell_pin = m_std_cells->pin_create(std_cell, m_pins.name(pin));
		m_pins.standard_cell_pin(pin, std_cell_pin);
	}
	return true;
}

} /* namespace netlist */
} /* namespace ophidian */

