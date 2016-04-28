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
#include "../interconnection/hpwl.h"

TEST_CASE("interconnection_estimation/hpwl test", "[interconnection][hpwl]") {
    std::vector< ophidian::geometry::point<double> > points
    {
        ophidian::geometry::point<double>{80, 50},
        ophidian::geometry::point<double>{140, 250},
        ophidian::geometry::point<double>{405, 40},
        ophidian::geometry::point<double>{207, 445}
    };

    double hpwl = ophidian::interconnection::hpwl(points);
    REQUIRE( hpwl == 730.0 );
}

