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

#include "../netlist/netlist.h"
#include "../entity/property.h"
#include "../interconnection/rc_tree.h"

#include <boost/units/systems/si/prefixes.hpp>

using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;

TEST_CASE("rc_tree_map","[timing][rc_tree][rc_tree_map]")
{

    standard_cell::standard_cells std_cells;
    netlist::netlist n{&std_cells};

    entity::vector_property<interconnection::rc_tree> tree_map;
    n.register_net_property(&tree_map);

    auto n1 = n.net_insert("n1");
    auto n2 = n.net_insert("n2");
    auto n3 = n.net_insert("n3");


    {
        auto & n1_tree = tree_map[n.net_system().lookup(n1)];
        auto & n2_tree = tree_map[n.net_system().lookup(n2)];
        auto & n3_tree = tree_map[n.net_system().lookup(n3)];

        auto n1_root = n1_tree.capacitor_insert("n1_root");
        auto n2_root = n2_tree.capacitor_insert("n2_root");
        auto n3_root = n3_tree.capacitor_insert("n3_root");

        n1_tree.capacitance(n1_root, quantity<capacitance>(1*femto*farad));
        n2_tree.capacitance(n2_root, quantity<capacitance>(2*femto*farad));
        n3_tree.capacitance(n3_root, quantity<capacitance>(3*femto*farad));
    }

    n.net_remove(n2);

    auto & n1_tree = tree_map[n.net_system().lookup(n1)];
    auto & n3_tree = tree_map[n.net_system().lookup(n3)];


    auto n1_root = n1_tree.capacitor_by_name("n1_root");
    auto n3_root = n3_tree.capacitor_by_name("n3_root");

    REQUIRE_NOTHROW( n1_tree.capacitance(n1_root) == quantity<capacitance>(1*femto*farad) );
    REQUIRE_NOTHROW( n1_tree.capacitance(n3_root) == quantity<capacitance>(3*femto*farad) );


}



