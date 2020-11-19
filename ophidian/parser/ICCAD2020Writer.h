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

#ifndef OPHIDIAN_PARSER_ICCAD2020_WRITER_H
#define OPHIDIAN_PARSER_ICCAD2020_WRITER_H

#include <ophidian/design/Design.h>
#include <fstream>

namespace ophidian::parser
{
    class ICCAD2020Writer
    {
    public:
        using design_type   = ophidian::design::Design;
        using point_type    = ophidian::util::LocationDbu;
        using cell_type     = ophidian::circuit::CellInstance;
        using movement_container_type = std::unordered_map< cell_type, point_type, ophidian::entity_system::EntityBaseHash>; 

        ICCAD2020Writer(design_type & design, std::string circuit_name);

        void write_ICCAD_2020_output(const std::string out_file_name, const movement_container_type & movements = {});

    private:
        design_type & m_design;
        std::string m_circuit_name;


    };    
}
#endif