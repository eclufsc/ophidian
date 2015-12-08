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

}

cell_identifier netlist_bgl::insert_cell(std::string name, std::string type) {
}

pin_identifier netlist_bgl::insert_pin(std::string name,
		cell_identifier owner) {
}

pin_identifier netlist_bgl::insert_pad(std::string name,
		direction_enum direction) {
}

net_identifier netlist_bgl::insert_net(std::string name) {
}

bool netlist_bgl::erase_cell(cell_identifier cell) {
	if(cell == -1)
		return false;
	return true;
}

bool netlist_bgl::erase_cell(std::string name) {
}

bool netlist_bgl::erase_net(net_identifier net) {
}

bool netlist_bgl::erase_net(std::string name) {
}

void netlist_bgl::connect(net_identifier net, pin_identifier pin) {
}

void netlist_bgl::disconnect(net_identifier net, pin_identifier pin) {
}

std::string netlist_bgl::cell_name(cell_identifier cell) {
}

std::string netlist_bgl::cell_type(cell_identifier cell) {
}

std::vector<pin_identifier> netlist_bgl::cell_pins(cell_identifier cell) {
}

std::string netlist_bgl::pin_name(pin_identifier pin) {
}

net_identifier netlist_bgl::pin_net(pin_identifier pin) {
}

cell_identifier netlist_bgl::pin_owner(pin_identifier pin) {
}

std::string netlist_bgl::net_name(net_identifier net) {
}

std::vector<pin_identifier> netlist_bgl::net_pins(net_identifier net) {
}

std::vector<pin_identifier> netlist_bgl::pads(direction_enum direction) {
}

std::size_t netlist_bgl::number_of_cells() {
	return 0;
}

std::size_t netlist_bgl::number_of_pins() {
	return 0;
}

std::size_t netlist_bgl::number_of_nets() {
	return 0;
}

std::size_t netlist_bgl::number_of_pads(direction_enum direction) {
}

} /* namespace netlist */
} /* namespace openeda */
