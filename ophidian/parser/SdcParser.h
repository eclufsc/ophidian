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
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::double_;
using ascii::space;
using x3::_attr;

namespace ophidian
{
namespace parser
{
namespace ast
{

struct GetPorts
{
  std::string expression;
};

struct AttrName
{
  std::string name;
};

struct AttrValue
{
  double value;
};

struct CreateClock
{
    std::string name;
    double period;
    std::string targetPorts;
};

struct SetInputDelay
{
    double inputDelay;
    std::string targetPorts;
    std::string clockName;
};

struct SetOutputDelay
{
    double outputDelay;
    std::string targetPorts;
    std::string clockName;
};

struct SetLoad
{
    double pinLoad;
    std::string targetPorts;
};

struct SetDrivingCell
{
    std::string libCell;
    std::string pinName;
    std::string targetPorts;
    double fallTransition;
    double riseTransition;
};

struct SdcCommands;
using SdcCommandsAst = x3::forward_ast<SdcCommands>;

struct SdcStatement : x3::variant<CreateClock, SetInputDelay, SdcCommandsAst>
{
    using base_type::base_type;
    using base_type::operator=;
};

struct SdcCommands
{
  std::vector<SdcStatement> commands;
/*    std::vector<CreateClock> createClocks;*/
    //std::vector<SetInputDelay> setInputDelays;
    //std::vector<SetOutputDelay> setOuputDelays;
    //std::vector<SetLoad> setLoads;
    /*std::vector<SetDrivingCell> setDrivingCells;*/
};

using boost::fusion::operator<<;


class Visitor final
{
public:
    using result_type = void;
    result_type operator()(const CreateClock & createClock) const
    {
      std::cout << "Creating Clock" << std::endl; 
    }
    result_type operator()(const SetInputDelay & setInputDelay) const
    {
      std::cout << "Setting Input Delay" << std::endl; 
    }
};

} //namespace ast
} //namespace parser
} //namespace ophidian

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::GetPorts,
    (std::string, expression)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::AttrName,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::AttrValue,
    (double, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::CreateClock,
    (std::string, name)
    (double, period)
    (std::string, targetPorts)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::SetInputDelay,
    (double, inputDelay)
    (std::string, targetPorts)
    (std::string, clockName)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::SetOutputDelay,
    (double, outputDelay)
    (std::string, targetPorts)
    (std::string, clockName)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::SetLoad,
    (double, pinLoad)
    (std::string, targetPorts)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::SetDrivingCell,
    (std::string, libCell)
    (std::string, pinName)
    (std::string, targetPorts)
    (double, fallTransition)
    (double, riseTransition)
)

BOOST_FUSION_ADAPT_STRUCT(
    ophidian::parser::ast::SdcCommands,
    (std::vector<ophidian::parser::ast::SdcStatement>, commands)
)

namespace ophidian
{
namespace parser
{

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::int_;
using x3::lit;
using x3::double_;
using x3::lexeme;
using ascii::char_;

//! Basic definition
x3::rule<class nonQuotedString, std::string> const nonQuotedString = "nonQuotedString";
x3::rule<class quotedString, std::string> const quotedString = "quotedString";
x3::rule<class stringValue, std::string> const stringValue = "stringValue";

//! Statement constructs
x3::rule<class getPorts, ast::GetPorts> const getPorts = "getPorts";
x3::rule<class clockName, ast::AttrName> clockName = "clockName";
x3::rule<class name, ast::AttrName> name = "name";
x3::rule<class libCell, ast::AttrName> libCell = "libCell";
x3::rule<class pinCell, ast::AttrName> pinCell = "pinCell";
x3::rule<class period, ast::AttrValue> period = "period";
x3::rule<class pinLoad, ast::AttrValue> pinLoad = "pinLoad";
x3::rule<class fallTransition, ast::AttrValue> fallTransition = "fallTransition";
x3::rule<class riseTransition, ast::AttrValue> riseTransition = "riseTransition";

//! Sdc Statements
x3::rule<class createClock, ast::CreateClock> const createClock = "createClock";
x3::rule<class setInputDelay, ast::SetInputDelay> const setInputDelay = "setInputDelay";
x3::rule<class setOutputDelay, ast::SetOutputDelay> const setOutputDelay = "setOutputDelay";
x3::rule<class setLoad, ast::SetLoad> const setLoad = "setLoad";
x3::rule<class setDrivingCell, ast::SetDrivingCell> const setDrivingCell = "setDrivingCell";

x3::rule<class sdcStatement, ast::SdcStatement> const sdcStatement = "sdcStatement";
//x3::rule<class sdcStatementList, std::vector<ast::SdcStatement> > sdcStatementList = "sdcStatementList";
x3::rule<class sdcCommands, ast::SdcCommands> const sdcCommands = "sdcCommands";

const auto quotedString_def     = lexeme['"' >> *(char_ - '"') >> '"'];
const auto nonQuotedString_def     = lexeme[*(char_)];
const auto stringValue_def      = (quotedString | nonQuotedString);
//const auto getPorts_def      = '[' >> lit("get_ports") >> - x3::string("{") >> stringValue >> - x3::string("}") >> ']';
const auto getPorts_def      = '[' >> lit("get_ports") >> stringValue >> ']';
const auto clockName_def      = lit("-clock") >> stringValue;
const auto name_def      = lit("-name") >> stringValue;
const auto libCell_def      = lit("-lib_cell") >> stringValue;
const auto pinCell_def      = lit("-pin") >> stringValue;
const auto period_def      = lit("-period") >> double_;
const auto pinLoad_def      = lit("-pin_load") >> double_;
const auto fallTransition_def      = lit("-input_transition_fall") >> double_;
const auto riseTransition_def      = lit("-input_transition_rise") >> double_;

const auto createClock_def      = lit("create_clock") >> name >> period >> getPorts;
const auto setInputDelay_def      = lit("set_input_delay") >> double_ >> getPorts >> clockName;
const auto setOutputDelay_def      = lit("set_output_delay") >> double_ >> getPorts >> clockName;
const auto setLoad_def      = lit("set_load") >> pinLoad >> getPorts;
const auto setDrivingCell_def      = lit("set_driving_cell") >> libCell >> pinCell >> getPorts >> fallTransition >> riseTransition;

const auto sdcStatement_def      = (createClock|setInputDelay);
//const auto sdcStatementList_def  = (*sdcStatement);
const auto sdcCommands_def      =  *sdcStatement;

BOOST_SPIRIT_DEFINE(quotedString, nonQuotedString, stringValue, getPorts, clockName, name, libCell, pinCell, period, pinLoad, fallTransition, riseTransition, createClock, setInputDelay, setOutputDelay, setLoad, setDrivingCell)

template <class Iterator>
bool parseCommand(const Iterator& first, const Iterator & last, ast::CreateClock & createClkArg)
{
  bool result = x3::phrase_parse(first, last, createClock, space, createClkArg);
  if(result && first == last)
  {
      return true;
  }
  return result;
}



template <class Iterator>
bool parseSdc(const Iterator& first, const Iterator & last, ast::SdcCommandsAst & sdcCommandsAst)
{
  bool result = x3::phrase_parse(first, last, sdcCommands, space, sdcCommandsAst.get());
  if(result && first == last)
  {
      return true;
  }
  return result;
}

} //namespace parser
} //namespace ophidian



#endif // SDCPARSER_H
