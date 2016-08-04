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

#include "../parsing/ops.h"

using namespace ophidian;
#include <iostream>


TEST_CASE("ops/read","[ops]") {

    parsing::ops reader("input_files/simple.ops");
    auto & register_pins_to_disconnect = reader.register_pins_to_disconnect();
    REQUIRE(register_pins_to_disconnect.size() == 1);
    REQUIRE(*register_pins_to_disconnect.begin() == "f1/ck");

    auto & pairs_to_connect = reader.pairs_of_register_pin_and_net_to_connect();
    REQUIRE(pairs_to_connect.size() == 1);
    REQUIRE((*pairs_to_connect.begin()).first == "f1/ck");
    REQUIRE((*pairs_to_connect.begin()).second == "lcb1_fo");

    std::cout << "disconnect: " << *register_pins_to_disconnect.begin() << std::endl;
    std::cout << "connect: " << (*pairs_to_connect.begin()).first << " to " << (*pairs_to_connect.begin()).second << std::endl;
}

