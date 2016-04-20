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

#include "../catch.hpp"
#include "../timing/spef.h"

#include <sstream>


#include <boost/units/systems/si/prefixes.hpp>

using namespace ophidian;
using namespace boost::units::si;
using namespace boost::units;

TEST_CASE("spef read", "[spef]")
{
    std::stringstream ss;
    ss << "*D_NET net_1 2.0\n\
          *CONN\n\
          *I inst_0:ZN O\n\
          *I inst_3:A2 I\n\
          *CAP\n\
          1 inst_0:ZN 0.5\n\
          3 inst_3:A2 1.5\n\
          *RES\n\
          1 inst_0:ZN inst_3:A2 0.0021\n\
          *END";
    timing::spef spef;
    spef.read(ss);

    REQUIRE(spef.trees().size() == 1);
    const timing::spef_tree & front = spef.trees().front();
    REQUIRE( front.net_name == "net_1" );
    REQUIRE( front.tree.capacitor_count() == 2 );
    REQUIRE( front.tree.lumped() == quantity<si::capacitance>(2.0*femto*farads) );
}
