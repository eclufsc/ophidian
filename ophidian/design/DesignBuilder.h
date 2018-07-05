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
#include <ophidian/parser/ParserException.h>
#include <ophidian/parser/VerilogParser.h>
#include <ophidian/floorplan/LefDef2Floorplan.h>
#include <ophidian/placement/Def2Placement.h>
#include <ophidian/placement/Lef2Library.h>
#include <ophidian/circuit/Def2LibraryMapping.h>
#include <ophidian/circuit/Verilog2Netlist.h>

#include "Design.h"

namespace ophidian
{
/// Creates a design using an input file
    namespace design
    {
        class DesignBuilder
        {
        public:
            virtual Design & build() = 0;
        };


        class ICCAD2017ContestDesignBuilder :
            public DesignBuilder
        {
        public:

            //! DesignBuilder Constructor

            /*!
               \brief Constructs a designBuilder
             */
            ICCAD2017ContestDesignBuilder(
                const std::string & cellLefFile,
                const std::string & techLefFile,
                const std::string & placedDefFile,
                const std::string & earlyLibFile,
                const std::string & lateLibFile,
                const std::string & designConstrainsFile);

            //! DesignBuilder Destructor

            /*!
               \brief Destroys the designBuilder, including its properties.
             */
            ~ICCAD2017ContestDesignBuilder();

            //! build a system with ICCAD2017 files

            /*!
               \brief build a system using 2 Lef and one Def as parameters.
               \return Design.
             */
            Design & build();

        private:
            design::Design                    mDesign;
            std::unique_ptr <parser::Lef>     mLef;
            std::unique_ptr <parser::Def>     mDef;
            std::shared_ptr <parser::Liberty> mEarlyLiberty;
            std::shared_ptr <parser::Liberty> mLateLiberty;
            std::shared_ptr <parser::DesignConstraints> mDC;
            std::string                       mCellLefFile;
            std::string                       mTechLefFile;
            std::string                       mPlacedDefFile;
            std::string                       mEarlyLibFile;
            std::string                       mLateLibFile;
            std::string                       mDesignConstrainsFile;
        };


        class ICCAD2015ContestDesignBuilder :
            public DesignBuilder
        {
        public:

            //! DesignBuilder Constructor

            /*!
               \brief Constructs a designBuilder
             */
            ICCAD2015ContestDesignBuilder(
                const std::string & lefFile,
                const std::string & defFile,
                const std::string & verilogFile,
                const std::string & earlyLibFile,
                const std::string & lateLibFile,
                const std::string & designConstrainsFile);

            //! DesignBuilder Destructor

            /*!
               \brief Destroys the designBuilder, including its properties.
             */
            ~ICCAD2015ContestDesignBuilder();

            //! build a system with ICCAD2015 files

            /*!
               \brief build a system using one verilog,one Lef and one Def as parameters.
               \return Design.
             */
            Design & build();

        private:
            design::Design                    mDesign;
            std::unique_ptr <parser::Lef>     mLef;
            std::unique_ptr <parser::Def>     mDef;
            std::unique_ptr <parser::Verilog> mVerilog;
            std::shared_ptr <parser::Liberty> mEarlyLiberty;
            std::shared_ptr <parser::Liberty> mLateLiberty;
            std::shared_ptr <parser::DesignConstraints> mDC;
            std::string                       mLefFile;
            std::string                       mDefFile;
            std::string                       mVerilogFile;
            std::string                       mEarlyLibFile;
            std::string                       mLateLibFile;
            std::string                       mDesignConstrainsFile;
        };
    }     //namespace design
}     //namespace ophidian

#endif // OPHIDIAN_DESIGN_DESIGNBUILDER_H
