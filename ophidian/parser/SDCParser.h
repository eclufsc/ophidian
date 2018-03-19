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

#ifndef OPHIDIAN_PARSER_SDCPARSER_H
#define OPHIDIAN_PARSER_SDCPARSER_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ParserException.h"

namespace ophidian
{
namespace parser
{

class DesignConstraints
{
public:
    struct Clock {
        double period;
        std::string name;
        std::string portName;
    };

    struct InputDelays {
        double delay;
        std::string clock;
        std::string portName;
    };

    struct DrivingCell {
        std::string portName;
        std::string libCell;
        std::string pinName;
        double slewFall;
        double slewRise;
    };

    struct OutputDelay {
        double delay;
        std::string clock;
        std::string portName;
    };

    struct OutputLoad {
        double pinLoad;
        std::string portName;
    };

    DesignConstraints();
    ~DesignConstraints();

//private: ???
    Clock mClock;
    std::vector<InputDelays> mInputDelays;
    std::vector<DrivingCell> mInputDrivers;
    std::vector<OutputDelay> mOutputDelays;
    std::vector<OutputLoad> mOutputLoads;
};

class SDCSimple
{
public:
    SDCSimple();
    std::shared_ptr<DesignConstraints> constraints() const;
};

class SDCParser
{
public:
    SDCParser();
    ~SDCParser();

    std::shared_ptr<DesignConstraints> readFile(const std::string & filename) const throw(InexistentFile);
};

}   // namespace parser
}   // namespace ophidian

#endif // OPHIDIAN_PARSER_SDCPARSER_H
