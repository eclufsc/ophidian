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

#ifndef OPHIDIAN_TIMING_ENDPOINTS_H
#define OPHIDIAN_TIMING_ENDPOINTS_H

#include "../entity/entity.h"
#include "../netlist/netlist.h"

namespace ophidian {
namespace timing {

class endpoints
{
    std::vector< entity::entity > m_entities;

public:
    endpoints();
    endpoints(const netlist::netlist & netlist);
    virtual ~endpoints();

    std::vector< entity::entity >::const_iterator begin() const {
        return m_entities.begin();
    }
    std::vector< entity::entity >::const_iterator end() const {
        return m_entities.end();
    }
    std::size_t size() const {
        return m_entities.size();
    }
    bool empty() const {
        return m_entities.empty();
    }

};

}
}

#endif // OPHIDIAN_TIMING_ENDPOINTS_H
