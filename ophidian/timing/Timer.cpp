/*
 * Copyright 2017 Ophidian
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

#include "Timer.h"
#include <ot/timer/timer.hpp>

namespace ophidian::timing
{
    float run_open_timer(){
        ot::Timer timer;
        timer.read_celllib("./input_files/tau2015/simple/simple_Late.lib", ot::Split::MAX);
        timer.read_celllib("./input_files/tau2015/simple/simple_Early.lib", ot::Split::MIN);
        timer.read_verilog("./input_files/tau2015/simple/simple.v");
        timer.read_spef("./input_files/tau2015/simple/simple.spef");
        timer.read_sdc("./input_files/tau2015/simple/simple.sdc");
        timer.update_timing();
        return *timer.wns();
    }
}
