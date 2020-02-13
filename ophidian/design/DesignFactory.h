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

#ifndef OPHIDIAN_DESIGN_DESIGNFACTORY_H
#define OPHIDIAN_DESIGN_DESIGNFACTORY_H

#include "Design.h"

#include <ophidian/parser/Verilog.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Guide.h>

namespace ophidian::design::factory
{
    void make_design(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept;

    void make_design_iccad2015(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Verilog& verilog) noexcept;

    void make_design_iccad2017(Design& design, const parser::Def& def, const parser::Lef& lef) noexcept;

    void make_design_ispd2018(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Guide &guide) noexcept;

    void make_design_ispd2019(Design& design, const parser::Def& def, const parser::Lef& lef, const parser::Guide &guide) noexcept;

    void make_design_iccad2019(Design& design, const parser::Def& def, const parser::Lef& lef) noexcept;
}

#endif // OPHIDIAN_DESIGN_DESIGNBUILDER_H
