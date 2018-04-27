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
    //--------------------------- Cells -------------------------------//

    StandardCells::cell_type StandardCells::add(StandardCells::cell_type, const StandardCells::cell_name_type& name)
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

    void StandardCells::erase(const StandardCells::cell_type& cell)
    {
        mName2Cell.erase(name(cell));
        mCells.erase(cell);
    }

    void StandardCells::reserve(StandardCells::cell_type, StandardCells::size_type size)
    {
        mCells.reserve(size);
    }

    StandardCells::size_type StandardCells::size(StandardCells::cell_type) const
    {
        return mCells.size();
    }

    StandardCells::size_type StandardCells::capacity(StandardCells::cell_type) const
    {
        return mCells.capacity();
    }

    StandardCells::cell_type StandardCells::find(StandardCells::cell_type, StandardCells::cell_name_type cellName)
    {
        return mName2Cell[cellName];
    }

    StandardCells::cell_name_type StandardCells::name(const StandardCells::cell_type& cell) const
    {
        return mCellNames[cell];
    }

    entity_system::Association<StandardCells::cell_type, StandardCells::pin_type>::Parts StandardCells::pins(const StandardCells::cell_type& cell) const
    {
        return mCellPins.parts(cell);
    }

    ophidian::util::Range<StandardCells::cell_const_iterator> StandardCells::range(StandardCells::cell_type) const
    {
        return ophidian::util::Range<StandardCells::cell_const_iterator>(mCells.begin(), mCells.end());
    }

    //--------------------------- Pins -------------------------------//

    StandardCells::pin_type StandardCells::add(StandardCells::pin_type, const StandardCells::pin_name_type& name, PinDirection direction)
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

    void StandardCells::erase(const StandardCells::pin_type& pin)
    {
        mName2Pin.erase(name(pin));
        mPins.erase(pin);
    }

    void StandardCells::reserve(StandardCells::pin_type, StandardCells::size_type size)
    {
        mPins.reserve(size);
    }

    StandardCells::size_type StandardCells::size(StandardCells::pin_type) const
    {
        return mPins.size();
    }

    StandardCells::size_type StandardCells::capacity(StandardCells::pin_type) const
    {
        return mPins.capacity();
    }

    StandardCells::pin_type StandardCells::find(StandardCells::pin_type, StandardCells::pin_name_type pinName)
    {
        return mName2Pin[pinName];
    }

    std::string StandardCells::name(const StandardCells::pin_type& pin) const
    {
        return mPinNames[pin];
    }

    PinDirection StandardCells::direction(const StandardCells::pin_type& pin)
    {
        return mPinDirections[pin];
    }

    StandardCells::cell_type StandardCells::owner(const StandardCells::pin_type& pin)
    {
        return mCellPins.whole(pin);
    }

    ophidian::util::Range<StandardCells::pin_const_iterator> StandardCells::range(StandardCells::pin_type) const
    {
        return ophidian::util::Range<StandardCells::pin_const_iterator>(mPins.begin(), mPins.end());
    }

    //--------------------------- Association -------------------------------//

    //! Add Pin into Cell

    /*!
       \brief Adds a Pin to a given Cell.
       \param cell A handler for the Cell we want to add a Pin.
       \param pin A handler for the Pin we want to add in \p cell.
     */
    void StandardCells::add(const StandardCells::cell_type& cell, const StandardCells::pin_type& pin)
    {
        mCellPins.addAssociation(cell, pin);
    }
}     //namespace ophidian
}     //namespace standard_cell
