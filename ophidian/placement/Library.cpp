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

namespace ophidian
{
namespace placement
{
    Library::Library(const standard_cell::StandardCells & std_cells):
            mGeometries(std_cells.makeProperty<geometry::MultiBox>(standard_cell::Cell())),
            mPinOffsets(std_cells.makeProperty<util::LocationDbu>(standard_cell::Pin()))
    {
    }

    void Library::geometry(const standard_cell::Cell & cell, const geometry::MultiBox & geometry)
    {
        mGeometries[cell] = geometry;
    }

    void Library::pinOffset(const standard_cell::Pin & pin, const util::LocationDbu & offset)
    {
        mPinOffsets[pin] = offset;
    }
}     // namespace placement
}     // namespace ophidian