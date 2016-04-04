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

#ifndef SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_
#define SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_

#include "design_constraints.h"

namespace ophidian {
namespace timing {

class simple_design_constraint {
	design_constraints m_dc;
public:
	simple_design_constraint();
	virtual ~simple_design_constraint();
	const design_constraints & dc() const {
		return m_dc;
	}
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_SIMPLE_DESIGN_CONSTRAINT_H_ */
