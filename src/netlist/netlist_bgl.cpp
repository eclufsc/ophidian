/*
 * This file is part of OpenEDA.
 * 
 * OpenEDA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenEDA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenEDA.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * netlist_bgl.cpp
 *
 *  Created on: Dec 8, 2015
 *      Author: csguth
 */

#include <netlist_bgl.h>

namespace openeda {
namespace netlist {

netlist_bgl::netlist_bgl() {
	insert_cell("___in_pads", "PI_PAD");
	insert_cell("___out_pads", "PO_PAD");
	insert_cell("___inout_pads", "PIO_PAD");
}

cell_identifier netlist_bgl::insert_cell(std::string name, std::string type) {
	cell_identifier id = find_cell_by_name(name);
	if(id != std::numeric_limits<cell_identifier>::max())
		return id;
	id = m_cells_pins.insert(std::vector<pin_identifier>());
	auto result = m_cells_names.insert(boost::bimap<std::string, cell_identifier>::value_type(name, id));
	assert (result.second);
	std::size_t type_id = std::numeric_limits<std::size_t>::max();
	m_cells_cell_types.resize(std::max(id+1, m_cells_cell_types.size()));
	auto type_it = m_cell_type_to_id.find(type);
	if (type_it == m_cell_type_to_id.end()) {
		type_id = m_cell_type_names.size();
		m_cell_type_names.push_back(type);
		type_it = m_cell_type_to_id.insert(std::make_pair(type, type_id)).first;
	}
	type_id = type_it->second;
	m_cells_cell_types[id] = type_id;
	return id;
}

pin_identifier netlist_bgl::insert_pin(std::string name,
		cell_identifier owner) {
	pin_identifier id = find_pin_by_name(name);
	if(id != std::numeric_limits<pin_identifier>::max())
		return id;

	id = m_pins_owners.insert(owner);
	auto result = m_pins_names.insert(boost::bimap<std::string, pin_identifier>::value_type(name, id));
	assert(result.second);

	if(id >= boost::num_vertices(m_graph))
		id = boost::add_vertex(m_graph);

	m_pins_nets.resize(std::max(m_pins_nets.size(), id+1));
	m_pins_nets[id] = std::numeric_limits<net_identifier>::max();
	m_cells_pins[owner].push_back(id);

	return id;
}

pin_identifier netlist_bgl::insert_pad(std::string name,
		direction_enum direction) {
	return insert_pin(name, static_cast<cell_identifier>(direction));
}

net_identifier netlist_bgl::insert_net(std::string name) {
	net_identifier id = find_net_by_name(name);
	if(id != std::numeric_limits<net_identifier>::max())
		return id;

	id = m_nets_pins.insert(std::vector<pin_identifier>());
	auto result = m_nets_names.insert(boost::bimap<std::string, net_identifier>::value_type(name, id));
	assert(result.second);

	return id;
}

void netlist_bgl::erase_cell(cell_identifier cell) {
	if (!m_cells_names.right.erase(cell))
		throw std::runtime_error("Cell not found!!");
	m_cells_pins.erase(cell);
}

void netlist_bgl::erase_cell(std::string name) {
	erase_cell(m_cells_names.left.at(name));
}

void netlist_bgl::erase_net(net_identifier net) {
	if (!m_nets_names.right.erase(net))
		throw std::runtime_error("Net not found!!");
	m_nets_pins.erase(net);
}

void netlist_bgl::erase_net(std::string name) {
	erase_net(m_nets_names.left.at(name));
}

void netlist_bgl::connect(net_identifier net, pin_identifier pin) {
	if(m_pins_nets[pin] != std::numeric_limits<net_identifier>::max())
		throw std::runtime_error("Pin must be disconnected!!");
	m_pins_nets[pin] = net;
	m_nets_pins[net].push_back(pin);
}

void netlist_bgl::disconnect(net_identifier net, pin_identifier pin) {
	if(m_pins_nets[pin] == std::numeric_limits<net_identifier>::max())
			throw std::runtime_error("Pin must be connected!!");
	auto & container = m_nets_pins[net];
	container.erase(std::remove(container.begin(), container.end(), pin));
	m_pins_nets[pin] = std::numeric_limits<net_identifier>::max();
}

std::string netlist_bgl::cell_name(cell_identifier cell) {
	return m_cells_names.right.at(cell);
}

std::string netlist_bgl::cell_type(cell_identifier cell) {
	return m_cell_type_names[m_cells_cell_types[cell]];
}

std::vector<pin_identifier> netlist_bgl::cell_pins(cell_identifier cell) {
	return m_cells_pins[cell];
}

std::string netlist_bgl::pin_name(pin_identifier pin) {
	return m_pins_names.right.at(pin);
}

net_identifier netlist_bgl::pin_net(pin_identifier pin) {
	return m_pins_nets[pin];
}

cell_identifier netlist_bgl::pin_owner(pin_identifier pin) {
	return m_pins_owners[pin];
}

std::string netlist_bgl::net_name(net_identifier net) {
	return m_nets_names.right.at(net);
}

std::vector<pin_identifier> netlist_bgl::net_pins(net_identifier net) {
	return m_nets_pins[net];
}

std::vector<pin_identifier> netlist_bgl::pads(direction_enum direction) {
	return m_cells_pins[static_cast<cell_identifier>(direction)];
}

std::size_t netlist_bgl::number_of_cells() {
	return m_cells_pins.size() - 3;
}

std::size_t netlist_bgl::number_of_pins() {
	return m_pins_owners.size();
}

std::size_t netlist_bgl::number_of_nets() {
	return m_nets_pins.size();
}

cell_identifier netlist_bgl::find_cell_by_name(std::string allocator) {
	auto result = m_cells_names.left.find(allocator);
	if(result != m_cells_names.left.end())
		return result->second;
	return std::numeric_limits<cell_identifier>::max();
}

pin_identifier netlist_bgl::find_pin_by_name(std::string allocator) {
	auto result = m_pins_names.left.find(allocator);
	if(result != m_pins_names.left.end())
		return result->second;
	return std::numeric_limits<pin_identifier>::max();
}

net_identifier netlist_bgl::find_net_by_name(std::string allocator) {
	auto result = m_nets_names.left.find(allocator);
	if(result != m_nets_names.left.end())
		return result->second;
	return std::numeric_limits<net_identifier>::max();
}

std::size_t netlist_bgl::number_of_pads(direction_enum direction) {
	std::size_t result = 0;
	for(std::size_t i = static_cast<std::size_t>(direction_enum::IN); i < static_cast<std::size_t>(direction_enum::INOUT); ++i)
		result += m_cells_pins[i].size();
	return result;
}

} /* namespace netlist */
} /* namespace openeda */
