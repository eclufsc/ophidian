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

#include "Library.h"

namespace ophidian::placement
{
    Library::Library(const circuit::StandardCells & std_cells):
            m_geometries{std_cells.make_property_cell<Library::std_cell_geometry_type>()},
            m_pin_offsets{std_cells.make_property_pin<Library::offset_type>()},
            m_pin_geometries{std_cells.make_property_pin<Library::std_pin_geometry_type>()}
    {}

    Library::std_cell_geometry_type& Library::geometry(const Library::std_cell_type& cell)
    {
        return m_geometries[cell];
    }

    const Library::std_cell_geometry_type& Library::geometry(const Library::std_cell_type& cell) const
    {
        return m_geometries[cell];
    }

    Library::offset_type& Library::offset(const Library::std_cell_pin_type& pin)
    {
        return m_pin_offsets[pin];
    }

    const Library::offset_type& Library::offset(const Library::std_cell_pin_type& pin) const
    {
        return m_pin_offsets[pin];
    }

    Library::std_pin_geometry_type& Library::geometry(const Library::std_cell_pin_type& pin)
    {
        return m_pin_geometries[pin];
    }

    const Library::std_pin_geometry_type& Library::geometry(const Library::std_cell_pin_type& pin) const
    {
        return m_pin_geometries[pin];
    }
}
