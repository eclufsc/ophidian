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

bool verilog::read_line_as_tokens(std::istream & in, std::vector<std::string> &tokens)
{
    tokens.clear();
    std::string line;
    std::getline(in, line);
    tokens.reserve(100);
    std::string token;
    token.reserve(100);
    while (in && tokens.empty())
    {
        token.clear();
        for (unsigned i = 0; i < line.size(); ++i)
        {
            char currChar = line[i];
            if (isspace(currChar) || is_special_char(currChar))
            {
                if (!token.empty())
                {
                    // Add the current token to the list of tokens
                    tokens.push_back(token);
                    token.clear();
                }
                // else if the token is empty, simply skip the whitespace or special char
            }
            else
            {
                // Add the char to the current token
                token.push_back(currChar);
            }
        }

        if (!token.empty())
            tokens.push_back(token);

        if (tokens.empty())
            // Previous line read was empty. Read the next one.
            std::getline(in, line);
    }

    return !tokens.empty();
}

bool verilog::read_module(std::istream &in, std::string &token)
{
    std::vector<std::string> tokens;
    bool valid = read_line_as_tokens(in, tokens);

    while (valid)
    {
        if (tokens.size() == 2 && tokens[0] == "module")
        {
            token = tokens[1];
            break;
        }
        valid = read_line_as_tokens(in, tokens);
    }

    // Read and skip the port names in the module definition
    // until we encounter the tokens {"Start", "PIs"}
    while (valid && !(tokens.size() == 2 && tokens[0] == "Start" && tokens[1] == "PIs"))
    {
        valid = read_line_as_tokens(in, tokens);
        assert(valid);
    }

    return valid;
}

bool verilog::read_primary_input(std::istream &in, std::string &token)
{
    token = "";

    std::vector<std::string> tokens;
    bool valid = read_line_as_tokens(in, tokens);

    assert(valid);
    assert(tokens.size() == 2);

    if (valid && tokens[0] == "input")
    {
        token = tokens[1];
    }
    else
    {
        assert (tokens[0] == "Start" && tokens[1] == "POs");
        return false;
    }

    return valid;
}

bool verilog::read_primary_output(std::istream &in, std::string &token)
{
    token = "";

    std::vector<std::string> tokens;
    bool valid = read_line_as_tokens(in, tokens);

    assert(valid);
    assert(tokens.size() == 2);

    if (valid && tokens[0] == "output")
    {
        token = tokens[1];
    }
    else
    {
        assert (tokens[0] == "Start" && tokens[1] == "wires");
        return false;
    }

    return valid;
}

bool verilog::read_cell_inst(std::istream &in, std::string &cell_type, std::string &cell_inst, std::vector<std::pair<std::string, std::string> > &pin_net_pairs)
{
    cell_type.clear();
    cell_inst.clear();
    pin_net_pairs.clear();
    std::vector<std::string> tokens;
    bool valid = read_line_as_tokens(in, tokens);
    assert(valid);

    if(tokens.size() == 1)
    {
        assert(tokens.front() == "endmodule");
        return false;
    }

    assert(tokens.size() >= 4);

    cell_type = tokens.at(0);
    cell_inst = tokens.at(1);

    pin_net_pairs.reserve(tokens.size());
    for(unsigned i = 2; i < tokens.size() - 1; i += 2)
    {
        assert(tokens.at(i).at(0) == '.');
        std::string pin_name = tokens.at(i).substr(1);
        pin_net_pairs.push_back(std::make_pair(pin_name, tokens.at(i+1)));
    }

    return valid;
}

bool verilog::read_wire(std::istream &in, std::string &token)
{
    token = "";

    std::vector<std::string> tokens;
    bool valid = read_line_as_tokens(in, tokens);

    assert(valid);
    assert(tokens.size() == 2);

    if (valid && tokens[0] == "wire")
    {
        token = tokens[1];
    }
    else
    {
        assert (tokens[0] == "Start" && tokens[1] == "cells");
        return false;
    }

    return valid;
}


void verilog::read(const std::string &filename)
{
    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();

    std::cout << "reading .v file..." << std::endl;

    std::ifstream file(filename);
    std::stringstream in;
    in << file.rdbuf();
    file.close();

    m_pin_count = 0;
    m_net_count = 0;

    bool valid = read_module(in, m_design);
    assert(valid);

    std::unordered_set<std::string> nets;


    std::string token;
    do
    {
        valid = read_primary_input(in, token);
        std::unordered_set<std::string> nets;
        if(valid)
        {
            m_inputs.push_back(token);
            m_pin_count++;
            nets.insert(token);
        }
    } while (valid);

    do
    {
        valid = read_primary_output(in, token);
        std::unordered_set<std::string> nets;
        if(valid)
        {
            m_outputs.push_back(token);
            m_pin_count++;
            nets.insert(token);
        }
    } while (valid);


    do
    {
        valid = read_wire(in, token);
        if(valid)
            nets.insert(token);
    } while (valid);




    do {
        std::string cellType, cellInst;
        std::vector<std::pair<std::string, std::string> > pinNetPairs;   /* (port name, net name) */

        valid = read_cell_inst(in, cellType, cellInst, pinNetPairs);
        if (valid)
        {
            for(auto pair : pinNetPairs)
                nets.insert(pair.second);
            module m;
            m.name = cellInst;
            m.type = cellType;
            m.pinnet_pairs = pinNetPairs;
            m_modules.push_back(m);
            m_pin_count += pinNetPairs.size();
        }

    } while(valid);

    m_wires.insert(m_wires.end(), nets.begin(), nets.end());
    m_net_count = m_wires.size();

    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration msdiff = mst2 - mst1;
    std::cout << "verilog::verilog(): " << msdiff.total_milliseconds() << " ms" << std::endl;

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

