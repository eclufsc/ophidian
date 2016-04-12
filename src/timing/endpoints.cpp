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

#include "endpoints.h"

namespace ophidian {
namespace timing {

endpoints::endpoints() {

}

endpoints::endpoints(const netlist::netlist & netlist) : m_entities(netlist.PO_count())
{
    m_entities.resize(0);
    for(auto PO = netlist.PO_begin(); PO != netlist.PO_end(); ++PO)
        m_entities.push_back(*PO);
    for(auto pin : netlist.pin_system())
    {
        auto pin_owner = netlist.pin_owner(pin.first);
        auto pin_std_cell = netlist.pin_std_cell(pin.first);
        if(pin_owner == entity::entity{}) continue;
        auto owner_std_cell = netlist.cell_std_cell(pin_owner);
        if(!netlist.std_cells().cell_sequential(owner_std_cell)) continue;
        if(netlist.std_cells().pin_direction(pin_std_cell) != standard_cell::pin_directions::INPUT) continue;
        if(netlist.std_cells().pin_clock_input(pin_std_cell)) continue;
        m_entities.push_back(pin.first);
    }
}

endpoints::~endpoints()
{

}

}
}
