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
#include "../interconnection/rc_tree.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>

#include <lemon/list_graph.h>
#include <lemon/bfs.h>
#include <lemon/connectivity.h>


#include "../timing/liberty.h"
#include <boost/units/cmath.hpp>


#include "../timing/ceff.h"
#include "../timing/elmore.h"
#include "../timing/elmore_second_moment.h"

using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;

struct simple_driver_model {
    timing::library & lib;
    entity_system::entity tarc;
    const quantity<si::time> slew;
    quantity<si::time> operator()(const quantity<capacitance> load) const {
        return lib.timing_arc_rise_slew(tarc).compute(load, slew);
    }
};
