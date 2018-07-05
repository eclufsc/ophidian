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

#include "StandardCells.h"

namespace ophidian
{
namespace circuit
{
    StandardCells::cell_type StandardCells::find_cell(StandardCells::cell_name_type cellName)
    {
        return mName2Cell[cellName];
    }

    const StandardCells::cell_type& StandardCells::find_cell(StandardCells::cell_name_type cellName) const
    {
        return mName2Cell.at(cellName);
    }

    StandardCells::pin_type StandardCells::find_pin(StandardCells::pin_name_type pinName)
    {
        return mName2Pin[pinName];
    }

    const StandardCells::pin_type& StandardCells::find_pin(StandardCells::pin_name_type pinName) const
    {
        return mName2Pin.at(pinName);
    }

    StandardCells::cell_name_type& StandardCells::name(const StandardCells::cell_type& cell)
    {
        return mCellNames[cell];
    }

    const StandardCells::cell_name_type& StandardCells::name(const StandardCells::cell_type& cell) const
    {
        return mCellNames[cell];
    }

    StandardCells::pin_name_type& StandardCells::name(const StandardCells::pin_type& pin)
    {
        return mPinNames[pin];
    }

    const StandardCells::pin_name_type& StandardCells::name(const StandardCells::pin_type& pin) const
    {
        return mPinNames[pin];
    }

    PinDirection& StandardCells::direction(const StandardCells::pin_type& pin)
    {
        return mPinDirections[pin];
    }

    const PinDirection& StandardCells::direction(const StandardCells::pin_type& pin) const
    {
        return mPinDirections[pin];
    }

    StandardCells::cell_type StandardCells::owner(const StandardCells::pin_type& pin) const
    {
        return mCellPins.whole(pin);
    }

    entity_system::Association<StandardCells::cell_type, StandardCells::pin_type>::Parts StandardCells::pins(const StandardCells::cell_type& cell) const
    {
        return mCellPins.parts(cell);
    }

    ophidian::util::Range<StandardCells::cell_const_iterator> StandardCells::range_cell() const
    {
        return ophidian::util::Range<StandardCells::cell_const_iterator>(mCells.begin(), mCells.end());
    }

    ophidian::util::Range<StandardCells::pin_const_iterator> StandardCells::range_pin() const
    {
        return ophidian::util::Range<StandardCells::pin_const_iterator>(mPins.begin(), mPins.end());
    }

    void StandardCells::reserve_cell(StandardCells::cell_container_type::size_type size)
    {
        mCells.reserve(size);
    }

    void StandardCells::reserve_pin(StandardCells::pin_container_type::size_type size)
    {
        mPins.reserve(size);
    }

    StandardCells::cell_container_type::size_type StandardCells::capacity_cell() const noexcept
    {
        return mCells.capacity();
    }

    StandardCells::pin_container_type::size_type StandardCells::capacity_pin() const noexcept
    {
        return mPins.capacity();
    }

    StandardCells::cell_container_type::size_type StandardCells::size_cell() const noexcept
    {
        return mCells.size();
    }

    StandardCells::pin_container_type::size_type StandardCells::size_pin() const noexcept
    {
        return mPins.size();
    }

    StandardCells::cell_type StandardCells::add_cell(const StandardCells::cell_name_type& name)
    {
        if(mName2Cell.find(name) == mName2Cell.end()) {
            auto cell = mCells.add();
            mCellNames[cell] = name;
            mName2Cell[name] = cell;

            return cell;
        }
        else {
            return mName2Cell[name];
        }
    }

    StandardCells::pin_type StandardCells::add_pin(const StandardCells::pin_name_type& name, PinDirection direction)
    {
        if(mName2Pin.find(name) == mName2Pin.end()) {
            auto pin = mPins.add();
            mPinNames[pin] = name;
            mPinDirections[pin] = direction;
            mName2Pin[name] = pin;

            return pin;
        }
        else {
            return mName2Pin[name];
        }
    }

    void StandardCells::connect(const StandardCells::cell_type& cell, const StandardCells::pin_type& pin)
    {
        mCellPins.addAssociation(cell, pin);
    }

    void StandardCells::erase(const StandardCells::cell_type& cell)
    {
        mName2Cell.erase(name(cell));
        mCells.erase(cell);
    }

    void StandardCells::erase(const StandardCells::pin_type& pin)
    {
        mName2Pin.erase(name(pin));
        mPins.erase(pin);
    }
}     //namespace ophidian
}     //namespace standard_cell
