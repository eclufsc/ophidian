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

#ifndef OPHIDIAN_CIRCUIT_VERILOG2NETLIST_H
#define OPHIDIAN_CIRCUIT_VERILOG2NETLIST_H

#include <ophidian/parser/VerilogParser.h>
#include <unordered_map>

#include "Netlist.h"

namespace ophidian
{
namespace circuit
{
    void verilog2Netlist(const parser::Verilog & verilog, circuit::Netlist & netlist);
}     // namespace circuit
}     // namespace ophidian

#endif // OPHIDIAN_CIRCUIT_VERILOG2NETLIST_H