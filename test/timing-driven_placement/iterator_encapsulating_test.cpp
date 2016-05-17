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
#include "../timing-driven_placement/timingdriven_placement.h"
using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;


TEST_CASE("iterate over nets", "[tdp][iterator][run]")
{

    timingdriven_placement::timingdriven_placement tdp("input_files/simple.v", "input_files/simple.def", "input_files/simple.lef", "input_files/simple_Late.lib", "input_files/simple_Early.lib", 80);

    std::cout << "circuit cells" << std::endl;
    for(timingdriven_placement::Cell cell : tdp.cells())
        std::cout << "  " << tdp.cell_name(cell) << std::endl;

    std::cout << "circuit pins" << std::endl;
    for(timingdriven_placement::Pin pin : tdp.pins())
        std::cout << "  " << tdp.pin_name(pin) << std::endl;

    std::cout << "circuit nets" << std::endl;
    for(timingdriven_placement::Net net : tdp.nets())
        std::cout << "  " << tdp.net_name(net) << std::endl;

}
