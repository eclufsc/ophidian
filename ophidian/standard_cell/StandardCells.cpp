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
namespace standard_cell
{


StandardCells::StandardCells() :
	mCellNames(mCells), mPinNames(mPins), mPinDirections(mPins),
	mCellPins(mCells, mPins)
{

}

StandardCells::StandardCells(const StandardCells && stdCell) :
	mCells(std::move(stdCell.mCells)),
	mPins(std::move(stdCell.mPins)),
	mCellNames(std::move(stdCell.mCellNames)),
	mPinNames(std::move(stdCell.mPinNames)),
	mPinDirections(std::move(stdCell.mPinDirections)),
	mCellPins(std::move(stdCell.mCellPins))
{

}

StandardCells::~StandardCells() = default;

//--------------------------- Cells -------------------------------//

Cell StandardCells::add(Cell, const std::string &name)
{
	if(mName2Cell.find(name) == mName2Cell.end())
	{
		auto cell = mCells.add();
		mCellNames[cell] = name;
		mName2Cell[name] = cell;
		return cell;
	}
	else {
		return mName2Cell[name];
	}
}

void StandardCells::erase(const Cell & cell)
{
	mName2Cell.erase(name(cell));
	mCells.erase(cell);
}

void StandardCells::reserve(Cell, uint32_t size)
{
	mCells.reserve(size);
}

uint32_t StandardCells::size(Cell) const
{
	return mCells.size();
}

uint32_t StandardCells::capacity(Cell) const
{
	return mCells.capacity();
}

Cell StandardCells::find(Cell, std::string cellName)
{
	return mName2Cell[cellName];
}

std::string StandardCells::name(const Cell & cell) const
{
	return mCellNames[cell];
}

entity_system::Association<Cell, Pin>::Parts StandardCells::pins(const Cell &cell) const
{
	return mCellPins.parts(cell);
}

ophidian::util::Range<StandardCells::CellsIterator> StandardCells::range(Cell) const
{
	return ophidian::util::Range<StandardCells::CellsIterator>(mCells.begin(), mCells.end());
}

//--------------------------- Pins -------------------------------//

Pin StandardCells::add(Pin, const std::string &name, PinDirection direction)
{
	if(mName2Pin.find(name) == mName2Pin.end())
	{
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

void StandardCells::erase(const Pin & pin)
{
	mName2Pin.erase(name(pin));
	mPins.erase(pin);
}

void StandardCells::reserve(Pin, uint32_t size)
{
	mPins.reserve(size);
}

uint32_t StandardCells::size(Pin) const
{
	return mPins.size();
}

uint32_t StandardCells::capacity(Pin) const
{
	return mPins.capacity();
}

Pin StandardCells::find(Pin, std::string pinName)
{
	return mName2Pin[pinName];
}

std::string StandardCells::name(const Pin & pin) const
{
	return mPinNames[pin];
}

PinDirection StandardCells::direction(const Pin & pin)
{
	return mPinDirections[pin];
}

Cell StandardCells::owner(const Pin & pin)
{
	return mCellPins.whole(pin);
}

ophidian::util::Range<StandardCells::PinsIterator> StandardCells::range(Pin) const
{
	return ophidian::util::Range<StandardCells::PinsIterator>(mPins.begin(), mPins.end());
}

//--------------------------- Association -------------------------------//

//! Add Pin into Cell
/*!
   \brief Adds a Pin to a given Cell.
   \param cell A handler for the Cell we want to add a Pin.
   \param pin A handler for the Pin we want to add in \p cell.
 */
void StandardCells::add(const Cell& cell, const Pin& pin)
{
	mCellPins.addAssociation(cell, pin);
}

} //namespace ophidian

} //namespace standard_cell
