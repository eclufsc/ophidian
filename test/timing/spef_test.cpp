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
