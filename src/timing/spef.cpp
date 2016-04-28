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

#include "spef.h"

#include <fstream>

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>

using namespace boost::units;
using namespace boost::units::si;

namespace ophidian {
namespace timing {

void spef::tokenize(const std::string &line, std::vector<std::string> &tokens)
{
    tokens.resize(0);
    std::string current_token;
    for(std::size_t i{0}; i < line.size(); ++i)
    {
        if(!std::isspace(line[i]) || line[i] == '\"')
            current_token.push_back(line[i]);
        else if(!current_token.empty())
        {
            tokens.push_back(current_token);
            current_token.clear();
        }
    }
    if(!current_token.empty())
        tokens.push_back(current_token);
}

void spef::skip_header(std::istream &in, std::vector<std::string> &tokens)
{
    tokens.resize(0);
    std::string line;
    while(!in.eof() && (tokens.size() <= 1 || tokens.front() != "*D_NET"))
    {
        std::getline(in, line);
        tokenize(line, tokens);
    }
}

bool spef::read_net(std::istream &in, std::vector<std::string> &tokens)
{
//    tokens.resize(0);
    std::string line;
    bool continue_reading = false;
    m_trees.push_back({tokens[1], interconnection::rc_tree{}});

    std::getline(in, line);
    tokenize(line, tokens);

    spef_tree & current_tree = m_trees[m_trees.size()-1];
    std::string source_name;

    while(!tokens.empty())
    {
        if(tokens.front() == "*CONN")
            source_name = read_conn(in, tokens, current_tree);
        else if(tokens.front() == "*CAP")
            read_cap(in, tokens, current_tree);
        else if(tokens.front() == "*RES")
            read_res(in, tokens, current_tree);
        else if(tokens.front() == "*END")
        {
            tokens.clear();
            while(!in.eof() && tokens.empty())
            {
                std::getline(in, line);
                tokenize(line, tokens);
            }
            if(tokens.front() == "*D_NET")
            {
                continue_reading = true;
                break;
            }
        }
    }

    current_tree.source = source_name;

    return continue_reading;
}

std::string spef::read_conn(std::istream &in, std::vector<std::string> &tokens, spef_tree &current_tree)
{
    std::string line;
    std::string source_name;
    while(tokens.front() != "*RES" && tokens.front() != "*CAP" && tokens.front() != "*END")
    {
        std::getline(in, line);
        tokenize(line, tokens);
        if(tokens.size() == 3)
        {
            auto cap = current_tree.tree.capacitor_insert(tokens[1]);
            if(is_source(tokens))
                source_name = tokens[1];
        }
    }
    return source_name;
}

void spef::read_cap(std::istream &in, std::vector<std::string> &tokens, spef_tree &current_tree)
{
    std::string line;
    while(tokens.front() != "*RES" && tokens.front() != "*CONN" && tokens.front() != "*END")
    {
        std::getline(in, line);
        tokenize(line, tokens);
        if(tokens.size() == 3)
        {
            auto cap = current_tree.tree.capacitor_insert(tokens[1]);
            current_tree.tree.capacitance(cap, quantity<capacitance>(std::stod(tokens[2])*femto*farads));
        }
    }
}

void spef::read_res(std::istream &in, std::vector<std::string> &tokens, spef_tree &current_tree)
{
    std::string line;
    while(tokens.front() != "*CONN" && tokens.front() != "*CAP" && tokens.front() != "*END")
    {
        std::getline(in, line);
        tokenize(line, tokens);
        if(tokens.size() == 4)
        {
            auto u = current_tree.tree.capacitor_insert(tokens[1]);
            auto v = current_tree.tree.capacitor_insert(tokens[2]);
            auto res = current_tree.tree.resistor_insert(u, v, quantity<resistance>(std::stod(tokens[3])*kilo*ohms));
        }
    }
}

bool spef::is_source(const std::vector<std::string> &tokens) const
{
    return tokens.front() == "*I" && tokens[2] == "O" ||
            tokens.front() == "*P" && tokens[2] == "I";
}


spef::spef()
{

}

void spef::read(const std::string &in)
{
    std::ifstream in_file(in.c_str(), std::ifstream::in);
    return read(in_file);
}

void spef::read(std::istream &in)
{
    std::string line;
    std::vector<std::string> tokens;
    tokens.reserve(10);
    skip_header(in, tokens);

    bool continue_reading{true};
    while(continue_reading)
        continue_reading = read_net(in, tokens);
}

}
}
