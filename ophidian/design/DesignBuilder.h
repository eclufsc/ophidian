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

#ifndef OPHIDIAN_DESIGN_DESIGNBUILDER_H
#define OPHIDIAN_DESIGN_DESIGNBUILDER_H

#include <fstream>
#include <sstream>
#include <ophidian/design/Design.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/parser/VerilogParser.h>
#include <ophidian/floorplan/LefDef2Floorplan.h>
#include <ophidian/placement/Def2Placement.h>
#include <ophidian/placement/Lef2Library.h>
#include <ophidian/circuit/Def2LibraryMapping.h>
#include <ophidian/circuit/Verilog2Netlist.h>

namespace ophidian
{

/// Creates a design using an input file
namespace designBuilder
{

class DesignBuilder
{
public:

	//! DesignBuilder Constructor
	/*!
	   \brief Constructs a designBuilder
	 */
	DesignBuilder();

	//! DesignBuilder Destructor
	/*!
	   \brief Destroys the designBuilder, including its properties.
	 */
	~DesignBuilder();

	//! build a system with ICCAD2017 files
	/*!
	   \brief build a system using 2 Lef and one Def as parameters
	 */
	void build2017(const std::string &cellLefFile, const std::string &techLefFile, const std::string &placedDefFile);

	//! build a system with ICCAD2015 files
	/*!
	   \brief build a system using 2 Lef and one Def as parameters
	 */
	void build2015(const std::string &lefFile, const std::string &defFile, const std::string &verilogFile);

	//! Design getter
	/*!
	   \brief Get the Design.
	   \return Design.
	 */

	design::Design & design()
	{
		return mDesign;
	}


private:
	design::Design mDesign;
	std::unique_ptr<parser::Lef> mLef;
	std::unique_ptr<parser::Def> mDef;
	std::unique_ptr<parser::Verilog> mVerilog;

};

} //namespace designBuilder

} //namespace ophidian

#endif // OPHIDIAN_DESIGN_DESIGNBUILDER_H
