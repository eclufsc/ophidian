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

#include "TimingParser.h"
#include <fstream>
#include <sstream>

namespace ophidian
{
namespace parser
{

std::size_t Timing::input(std::string name){
    auto inpIt = name2Input.find(name);
    if(inpIt == name2Input.end())
    {
        inputs.push_back(Input(name));
        auto input_index = inputs.size()-1;
        name2Input[name] = input_index;
        return input_index;
    }else
        return inpIt->second;
}

std::size_t Timing::output(std::string name){
    auto outIt = name2Output.find(name);
    if(outIt == name2Output.end())
    {
        outputs.push_back(Output(name));
        auto output_index = outputs.size()-1;
        name2Output[name] = output_index;
        return output_index;
    }else
        return outIt->second;
}

Timing * TimingParser::readStream(std::istream & in){
    std::string line, token, portName;
    while(std::getline(in, line))
    {
        std::stringstream ss(line);
        ss >> token;

        if(token == "clock")
            ss >> mTiming->clkName >> mTiming->clkPeriod;
        else if(token == "at")
        {
            float at[2][2];
            ss >> portName >> at[timing::EARLY_MODE][timing::RISE_TRANSITION] >> at[timing::EARLY_MODE][timing::FALL_TRANSITION] >> at[timing::LATE_MODE][timing::RISE_TRANSITION] >> at[timing::LATE_MODE][timing::FALL_TRANSITION];
            auto inputIndex = mTiming->input(portName);
            mTiming->inputs.at(inputIndex).at[timing::EARLY_MODE][timing::RISE_TRANSITION] = at[timing::EARLY_MODE][timing::RISE_TRANSITION];
            mTiming->inputs.at(inputIndex).at[timing::EARLY_MODE][timing::FALL_TRANSITION] = at[timing::EARLY_MODE][timing::FALL_TRANSITION];
            mTiming->inputs.at(inputIndex).at[timing::LATE_MODE][timing::RISE_TRANSITION] = at[timing::LATE_MODE][timing::RISE_TRANSITION];
            mTiming->inputs.at(inputIndex).at[timing::LATE_MODE][timing::FALL_TRANSITION] = at[timing::LATE_MODE][timing::FALL_TRANSITION];
        }else if(token == "slew")
        {
            float slew[2][2];
            ss >> portName >> slew[timing::EARLY_MODE][timing::RISE_TRANSITION] >> slew[timing::EARLY_MODE][timing::FALL_TRANSITION] >> slew[timing::LATE_MODE][timing::RISE_TRANSITION] >> slew[timing::LATE_MODE][timing::FALL_TRANSITION];
            auto inputIndex = mTiming->input(portName);
            mTiming->inputs.at(inputIndex).slew[timing::EARLY_MODE][timing::RISE_TRANSITION] = slew[timing::EARLY_MODE][timing::RISE_TRANSITION];
            mTiming->inputs.at(inputIndex).slew[timing::EARLY_MODE][timing::FALL_TRANSITION] = slew[timing::EARLY_MODE][timing::FALL_TRANSITION];
            mTiming->inputs.at(inputIndex).slew[timing::LATE_MODE][timing::RISE_TRANSITION] = slew[timing::LATE_MODE][timing::RISE_TRANSITION];
            mTiming->inputs.at(inputIndex).slew[timing::LATE_MODE][timing::FALL_TRANSITION] = slew[timing::LATE_MODE][timing::FALL_TRANSITION];
        }else if(token == "rat")
        {
            float rat[2][2];
            ss >> portName >> rat[timing::EARLY_MODE][timing::RISE_TRANSITION] >> rat[timing::EARLY_MODE][timing::FALL_TRANSITION] >> rat[timing::LATE_MODE][timing::RISE_TRANSITION] >> rat[timing::LATE_MODE][timing::FALL_TRANSITION];
            auto outputIndex = mTiming->output(portName);
            mTiming->outputs.at(outputIndex).rat[timing::EARLY_MODE][timing::RISE_TRANSITION] = rat[timing::EARLY_MODE][timing::RISE_TRANSITION];
            mTiming->outputs.at(outputIndex).rat[timing::EARLY_MODE][timing::FALL_TRANSITION] = rat[timing::EARLY_MODE][timing::FALL_TRANSITION];
            mTiming->outputs.at(outputIndex).rat[timing::LATE_MODE][timing::RISE_TRANSITION] = rat[timing::LATE_MODE][timing::RISE_TRANSITION];
            mTiming->outputs.at(outputIndex).rat[timing::LATE_MODE][timing::FALL_TRANSITION] = rat[timing::LATE_MODE][timing::FALL_TRANSITION];
        }else if(token == "load")
        {
            float load;
            ss >> portName >> load;
            auto outputIndex = mTiming->output(portName);
            mTiming->outputs.at(outputIndex).load = load;
        }
    }
    return mTiming.release();
}

Timing * TimingParser::readFile(const std::string & filename)
{
    std::ifstream input(filename.c_str(), std::ifstream::in);
    if(!input.good())
        return nullptr;
    auto timingPtr = readStream(input);
    input.close();
    return timingPtr;
}

} // namespace parser
} // namespace ophidian
