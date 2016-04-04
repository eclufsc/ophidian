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

#include "hpwl.h"

namespace ophidian {
    namespace interconnection {
        double hpwl(const std::vector<geometry::point<double> >& points) {
            geometry::point<double> lower( std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
            geometry::point<double> upper( std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() );
            for(auto & p : points)
            {
                lower.x(std::min(lower.x(), p.x()));
                lower.y(std::min(lower.y(), p.y()));
                upper.x(std::max(upper.x(), p.x()));
                upper.y(std::max(upper.y(), p.y()));
            }
            return ophidian::geometry::manhattan_distance(lower, upper);
        }
    }
}
