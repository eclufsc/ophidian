/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */


#include "../catch.hpp"

#include "../netlist/netlist.h"
#include "../entity_system/vector_property.h"
#include "../entity_system/entity.h"
#include "../interconnection/rc_tree.h"

#include <boost/units/systems/si/prefixes.hpp>

using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;

TEST_CASE("rc_tree_map","[timing][rc_tree][rc_tree_map]")
{

    standard_cell::standard_cells std_cells;
    netlist::netlist n{&std_cells};

    entity_system::vector_property<interconnection::rc_tree> tree_map;
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



