/*
 * Copyright 2016 Ophidian
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

#include "verilog.h"

#include <boost/date_time/posix_time/posix_time.hpp>


#include <iostream>
#include <fstream>
#include <unordered_set>
#include <algorithm>


namespace ophidian {
namespace parsing {

bool is_special_char(char c) {
    static const char specialChars[] = { '(', ')', ',', ':', ';', '/', '#', '[',
                                         ']', '{', '}', '*', '\"', '\\' };

    for (unsigned i = 0; i < sizeof(specialChars); ++i) {
        if (c == specialChars[i])
            return true;
    }

    return false;
}

std::vector<std::string> verilog::tokenize(std::string line) {
    std::vector<std::string> tokens;
    //    tokens.reserve(num_spaces+1);
    std::string last_token;
    for (auto c : line) {
        if (isspace(c) || is_special_char(c)) {
            if (!last_token.empty()) {
                tokens.push_back(last_token);
                last_token.clear();
            }
        } else
            last_token.push_back(c);
    }
    if (!last_token.empty())
        tokens.push_back(last_token);
    return tokens;
}


void verilog::read(const std::string &filename)
{
    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();

    std::cout << "reading .v file..." << std::endl;

    std::ifstream file(filename);
    std::stringstream in;
    in << file.rdbuf();
    file.close();


    read_module(in);

    std::string line;
    std::vector<std::string> tokens;
    do {
        std::getline(in, line);
        tokens = tokenize(line);
    } while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "PIs");



    std::vector<std::string> strings;
    strings.reserve(1024);

    std::unordered_set<std::string> unique_net_names;
    std::unordered_set<std::string> unique_pin_names;

    // PI
    do {
        std::getline(in, line);
        tokens = tokenize(line);
        if (tokens.size() == 2 && tokens[0] == "input")
            strings.push_back(tokens[1]);
    } while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "POs");

    unique_pin_names.insert(strings.begin(), strings.end());
    m_inputs=strings;
    strings.resize(0);

    // PO
    do {
        std::getline(in, line);
        tokens = tokenize(line);
        if (tokens.size() == 2 && tokens[0] == "output")
            strings.push_back(tokens[1]);
    } while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "wires");

    unique_pin_names.insert(strings.begin(), strings.end());
    m_outputs=strings;
    strings.resize(0);

    // PO
    do {
        std::getline(in, line);
        tokens = tokenize(line);
        if (tokens.size() == 2 && tokens[0] == "wire")
            strings.push_back(tokens[1]);
    } while (tokens.size() != 2 || tokens[0] != "Start" || tokens[1] != "cells");

    unique_pin_names.insert(strings.begin(), strings.end());
    m_wires=strings;
    strings.resize(0);


    std::vector<module> modules;
    do {
        std::getline(in, line);
        tokens = tokenize(line);
        if(tokens.empty())
            continue;
        if(tokens.size() == 1 && tokens[0] == "endmodule")
            break;
        module m;
        m.type = tokens[0];
        m.name = tokens[1];
        m.pinnet_pairs.reserve((tokens.size()-2)<<1);
        for(std::size_t i = 2; i < tokens.size()-1; i+=2)
        {
            auto pin_name = tokens[i].substr(1);
            m.pinnet_pairs.push_back(std::make_pair(pin_name, tokens[i+1]));
            unique_pin_names.insert(m.name + ":" + pin_name);
            unique_net_names.insert(tokens[i+1]);
        }
        modules.push_back(m);
    } while (tokens.size() != 1 || tokens[0] != "endmodule");
    m_modules=modules;
    std::cout << "reading .v file DONE" << std::endl<< std::flush;

    m_net_count = unique_net_names.size();
    m_pin_count = unique_pin_names.size();

    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration msdiff = mst2 - mst1;
    std::cout << "verilog::verilog(): " << msdiff.total_milliseconds() << " ms" << std::endl;

}

void verilog::read_module(std::istream &in)
{
    std::vector<std::string> tokens;
    std::string line;
    bool valid = false;
    std::getline(in, line);
    tokens = tokenize(line);
    valid = !tokens.empty();
    while (valid)
    {
        if (tokens.size() == 2 && tokens[0] == "module")
        {
            m_design = tokens[1];
            break;
        }
        std::getline(in, line);
        tokens = tokenize(line);
        valid = !tokens.empty();

    }

    // Read and skip the port names in the module definition
    // until we encounter the tokens {"Start", "PIs"}
    while (valid && !(tokens.size() == 2 && tokens[0] == "Start" && tokens[1] == "PIs"))
    {
        std::getline(in, line);
        tokens = tokenize(line);
        valid = !tokens.empty();
        assert(valid);
    }
    assert(valid);
}

verilog::verilog(const std::string &filename) :
    m_pin_count(0),
    m_net_count(0)
{
    read(filename);
}

verilog::~verilog()
{

}

}
}

