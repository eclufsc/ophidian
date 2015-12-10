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
 * netlist_bgl.h
 *
 *  Created on: Dec 8, 2015
 *      Author: csguth
 */

#ifndef SRC_NETLIST_NETLIST_BGL_H_
#define SRC_NETLIST_NETLIST_BGL_H_

#include <netlist_interface.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bimap.hpp>
#include <unordered_map>
#include <memory>
#include <unordered_set>

#include "../data_structures/recycling_vector.h"


namespace openeda {
namespace netlist {

class netlist_bgl: public netlist_interface {

	boost::adjacency_list<> m_graph;

	// cells
	boost::bimap<std::string, cell_identifier> m_cells_names;
	data_structures::recycling_vector<std::vector<pin_identifier>, cell_identifier> m_cells_pins;
	std::vector<std::size_t> m_cells_cell_types;

	// cell types
	std::vector<std::string> m_cell_type_names;
	std::unordered_map<std::string, std::size_t> m_cell_type_to_id;

	// pins
	boost::bimap<std::string, pin_identifier> m_pins_names;
	data_structures::recycling_vector<cell_identifier, pin_identifier> m_pins_owners;
	std::vector<net_identifier> m_pins_nets;

	// net
	boost::bimap<std::string, net_identifier> m_nets_names;
	data_structures::recycling_vector<std::vector<pin_identifier>, net_identifier> m_nets_pins;

	cell_identifier find_cell_by_name(std::string);
	pin_identifier find_pin_by_name(std::string);
	net_identifier find_net_by_name(std::string);

public:
	virtual ~netlist_bgl() {
	}
	netlist_bgl();

	virtual cell_identifier insert_cell(std::string name, std::string type);
	virtual pin_identifier insert_pin(std::string name, cell_identifier owner);
	virtual pin_identifier insert_pad(std::string name,
			direction_enum direction);
	virtual net_identifier insert_net(std::string name);

	virtual void erase_cell(cell_identifier cell);
	virtual void erase_cell(std::string name);

	virtual void erase_net(net_identifier net);
	virtual void erase_net(std::string name);

	virtual void connect(net_identifier net, pin_identifier pin);
	virtual void disconnect(net_identifier net, pin_identifier pin);

	virtual std::string cell_name(cell_identifier cell);
	virtual std::string cell_type(cell_identifier cell);
	virtual std::vector<pin_identifier> cell_pins(cell_identifier cell);

	virtual std::string pin_name(pin_identifier pin);
	virtual net_identifier pin_net(pin_identifier pin);
	virtual cell_identifier pin_owner(pin_identifier pin);

	virtual std::string net_name(net_identifier net);
	virtual std::vector<pin_identifier> net_pins(net_identifier net);

	virtual std::vector<pin_identifier> pads(direction_enum direction);

	virtual std::size_t number_of_cells();
	virtual std::size_t number_of_pins();
	virtual std::size_t number_of_nets();
	virtual std::size_t number_of_pads(direction_enum direction);

};

} /* namespace netlist */
} /* namespace openeda */

#endif /* SRC_NETLIST_NETLIST_BGL_H_ */
