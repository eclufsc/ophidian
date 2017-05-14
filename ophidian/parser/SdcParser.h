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

#ifndef SDCPARSER_H
#define SDCPARSER_H

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


#include <string>
#include <vector>

namespace ophidian
{
namespace parser
{

namespace ast
{

struct Command
{
//    using Value = boost::variant<double, int>;
    std::string command;
    std::string name;
    double period;
};

} //namespace ast


} //namespace parser
} //namespace ophidian

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::Command,
    (std::string, command)
    (std::string, name)
    (double, period)
)


#endif // SDCPARSER_H
