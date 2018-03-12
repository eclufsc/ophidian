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

#include "DesignBuilder.h"

namespace ophidian
{
namespace design
{

    ICCAD2017ContestDesignBuilder::ICCAD2017ContestDesignBuilder(
        const std::string & cellLefFile,
        const std::string & techLefFile,
        const std::string & placedDefFile):
            mDesign(),
            mLef(),
            mDef(),
            mCellLefFile(cellLefFile),
            mTechLefFile(techLefFile),
            mPlacedDefFile(placedDefFile)
    {
    }

    ICCAD2017ContestDesignBuilder::~ICCAD2017ContestDesignBuilder()
    {
    }

    Design & ICCAD2017ContestDesignBuilder::build()
    {
        parser::LefParser lefParser;
        parser::DefParser defParser;

        mLef = std::make_unique<ophidian::parser::Lef>();
        lefParser.readFile(mCellLefFile, mLef);
        lefParser.readFile(mTechLefFile, mLef);

        mDef = defParser.readFile(mPlacedDefFile);

        placement::def2placement(*mDef, mDesign.placement(), mDesign.netlist());
        floorplan::lefDef2Floorplan(*mLef, *mDef, mDesign.floorplan());
        placement::lef2Library(*mLef, mDesign.library(), mDesign.standardCells());
        circuit::def2LibraryMapping(
            *mDef,
            mDesign.netlist(),
            mDesign.standardCells(),
            mDesign.libraryMapping());

        return mDesign;
    }

    ICCAD2015ContestDesignBuilder::ICCAD2015ContestDesignBuilder(
        const std::string & lefFile,
        const std::string & defFile,
        const std::string & verilogFile):
            mDesign(),
            mLef(),
            mDef(),
            mVerilog(),
            mLefFile(lefFile),
            mDefFile(defFile),
            mVerilogFile(verilogFile)
    {
    }

    ICCAD2015ContestDesignBuilder::~ICCAD2015ContestDesignBuilder()
    {
    }

    Design & ICCAD2015ContestDesignBuilder::build()
    {
        parser::LefParser     lefParser;
        parser::DefParser     defParser;
        parser::VerilogParser vParser;

        mLef = std::make_unique<ophidian::parser::Lef>();
        lefParser.readFile(mLefFile, mLef);

        mDef = defParser.readFile(mDefFile);

        mVerilog.reset(vParser.readFile(mVerilogFile));


        placement::lef2Library(*mLef, mDesign.library(), mDesign.standardCells());
        floorplan::lefDef2Floorplan(*mLef, *mDef, mDesign.floorplan());
        placement::def2placement(*mDef, mDesign.placement(), mDesign.netlist());
        circuit::verilog2Netlist(*mVerilog, mDesign.netlist());

        return mDesign;
    }
}     //namespace design
}     //namespace ophidian