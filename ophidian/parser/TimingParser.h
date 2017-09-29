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

#ifndef TIMINGPARSER_H
#define TIMINGPARSER_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <ophidian/util/TimingTypes.h>

namespace ophidian
{
namespace parser
{

class TimingParser;

struct Input
{
    Input(std::string name):name(name){}
    std::string name;
    float at[2][2], slew[2][2];
};

struct Output
{
    Output(std::string name):name(name){}
    std::string name;
    float rat[2][2], load;
};

class Timing{
friend class TimingParser;
public:
    Timing(){}
    float clockPeriod() const {return clkPeriod;}
    const std::string& clockName() const {return clkName;}

    float at(std::string inpName, timing::mode mode, timing::transition transition){
        auto inpIt = name2Input.find(inpName);
        return inputs.at(inpIt->second).at[mode][transition];
    }
    float slew(std::string inpName, timing::mode mode, timing::transition transition){
        auto inpIt = name2Input.find(inpName);
        return inputs.at(inpIt->second).slew[mode][transition];
    }
    float rat(std::string inpName, timing::mode mode, timing::transition transition){
        auto outIt = name2Output.find(inpName);
        return outputs.at(outIt->second).rat[mode][transition];
    }
    float load(std::string inpName){
        auto outIt = name2Output.find(inpName);
        return outputs.at(outIt->second).load;
    }


private:
    std::size_t input(std::string name);
    std::size_t output(std::string name);
    std::vector<Input> inputs;
    std::vector<Output> outputs;
    std::unordered_map<std::string, std::size_t> name2Input;
    std::unordered_map<std::string, std::size_t> name2Output;
    std::string clkName;
    float clkPeriod;
};

class TimingParser{
public:
    TimingParser():mTiming(new Timing){}

    Timing * readStream(std::istream & in);
    Timing * readFile(const std::string & filename);
private:
    std::unique_ptr<Timing> mTiming;
};

} // namespace parser
} // namespace ophidian

#endif // TIMINGPARSER_H
