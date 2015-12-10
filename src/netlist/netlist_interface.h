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
 * netlist_interface.h
 *
 *  Created on: Dec 8, 2015
 *      Author: csguth
 */

#ifndef OPENEDA_NETLIST_NETLIST_INTERFACE_H_
#define OPENEDA_NETLIST_NETLIST_INTERFACE_H_

#include <string>
#include <vector>

namespace openeda {
namespace netlist {

typedef std::size_t cell_identifier;
typedef std::size_t net_identifier;
typedef std::size_t pin_identifier;

enum class direction_enum  {
	IN=0, OUT, INOUT
};

class netlist_interface {
public:

	virtual ~netlist_interface() { }

	virtual cell_identifier insert_cell(std::string name, std::string type) = 0;
	virtual pin_identifier insert_pin(std::string name, cell_identifier owner) = 0;
	virtual pin_identifier insert_pad(std::string name, direction_enum direction) = 0;
	virtual net_identifier insert_net(std::string name) = 0;

	virtual void erase_cell(cell_identifier cell) = 0;
	virtual void erase_cell(std::string name) = 0;

	virtual void erase_net(net_identifier net) = 0;
	virtual void erase_net(std::string name) = 0;

	virtual void connect(net_identifier net, pin_identifier pin) = 0;
	virtual void disconnect(net_identifier net, pin_identifier pin) = 0;

	virtual std::string cell_name(cell_identifier cell) = 0;
	virtual std::string cell_type(cell_identifier cell) = 0;
	virtual std::vector<pin_identifier> cell_pins(cell_identifier cell) = 0;

	virtual std::string pin_name(pin_identifier pin) = 0;
	virtual net_identifier pin_net(pin_identifier pin) = 0;
	virtual cell_identifier pin_owner(pin_identifier pin) = 0;

	virtual std::string net_name(net_identifier net) = 0;
	virtual std::vector<pin_identifier> net_pins(net_identifier net) = 0;

	virtual std::vector<pin_identifier> pads(direction_enum direction) = 0;

	virtual std::size_t number_of_cells() = 0;
	virtual std::size_t number_of_pins() = 0;
	virtual std::size_t number_of_nets() = 0;
	virtual std::size_t number_of_pads(direction_enum direction) = 0;

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* OPENEDA_NETLIST_NETLIST_INTERFACE_H_ */
