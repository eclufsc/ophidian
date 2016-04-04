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

#include "stwl.h"
#include "hpwl.h"
#include "flute.h"

namespace ophidian {
    namespace interconnection {
        double stwl(const std::vector<geometry::point<double> >&points) {
            if(points.size() <= 3)
                return hpwl(points);
            std::vector<unsigned> X(points.size());
            std::vector<unsigned> Y(points.size());
            X.resize(0); Y.resize(0);
            for (auto & p : points) {
                X.push_back(p.x());
                Y.push_back(p.y());
            }
            auto tree = flute(points.size(), X.data(), Y.data(), ACCURACY);
            delete tree.branch;
            return tree.length;
        }
    }
}
