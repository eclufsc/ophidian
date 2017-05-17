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

#include <catch.hpp>
#include <iostream>
#include <string>

#include "SdcParser_test.h"

TEST_CASE("Sdc Parser: nonQuotedString", "[parser][SdcParser]")
{
  const std::string input = "non quoted string ";
  auto first = input.begin();
  auto last = input.end();
  std::string result;
  CHECK(parseAttribute(first, last, nonQuotedString, result)); 
  REQUIRE(result == "non quoted string ");
}

TEST_CASE("Sdc Parser: quotedString", "[parser][SdcParser]")
{
  const std::string input = "\"quoted string \"";
  auto first = input.begin();
  auto last = input.end();
  std::string result;
  CHECK(parseAttribute(first, last, quotedString, result)); 
  REQUIRE(result == "quoted string ");
}

TEST_CASE("Sdc Parser: stringValue", "[parser][SdcParser]")
{
  const std::string input = "non quoted string ";
  auto first = input.begin();
  auto last = input.end();
  std::string result;
  CHECK(parseAttribute(first, last, stringValue, result)); 
  REQUIRE(result == "non quoted string ");

  const std::string input2 = " quoted string ";
  auto first2 = input2.begin();
  auto last2 = input2.end();
  std::string result2;
  CHECK(parseAttribute(first2, last2, stringValue, result2)); 
  REQUIRE(result2 == " quoted string ");
}

TEST_CASE("Sdc Parser: getPorts", "[parser][SdcParser]")
{
  std::string input = " [get_ports iccad_clk]";
  auto first = input.begin();
  auto last = input.end();
  ast::GetPorts result;
  CHECK(parsePhrase(first, last, getPorts, space, result));
  std::cout << "Result: " << result << std::endl;
  std::cout << "Result: " << result.expression << std::endl;
  //REQUIRE(result.expression == "iccad_clk");
}

TEST_CASE("Sdc Parser: clockName", "[parser][SdcParser]")
{
  std::string input = "-clock masterClock";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrName result;
  CHECK(parsePhrase(first, last, clockName, space, result));
  REQUIRE(result.name == "masterClock");
}

TEST_CASE("Sdc Parser: name", "[parser][SdcParser]")
{
  std::string input = "-name mclk";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrName result;
  CHECK(parsePhrase(first, last, name, space, result));
  REQUIRE(result.name == "mclk");
}

TEST_CASE("Sdc Parser: libCell", "[parser][SdcParser]")
{
  std::string input = "-lib_cell INV_X80";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrName result;
  CHECK(parsePhrase(first, last, libCell, space, result));
  REQUIRE(result.name == "INV_X80");
}

TEST_CASE("Sdc Parser: pinCell", "[parser][SdcParser]")
{
  std::string input = "-pin o";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrName result;
  CHECK(parsePhrase(first, last, pinCell, space, result));
  REQUIRE(result.name == "o");
}

TEST_CASE("Sdc Parser: period", "[parser][SdcParser]")
{
  std::string input = "-period 7000.0";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrValue result;
  CHECK(parsePhrase(first, last, period, space, result));
  REQUIRE(result.value == 7000.0);
}

TEST_CASE("Sdc Parser: pinLoad", "[parser][SdcParser]")
{
  std::string input = "-pin_load 4.0";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrValue result;
  CHECK(parsePhrase(first, last, pinLoad, space, result));
  REQUIRE(result.value == 4.0);
}

TEST_CASE("Sdc Parser: fallTransition", "[parser][SdcParser]")
{
  std::string input = "-input_transition_fall 10.0";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrValue result;
  CHECK(parsePhrase(first, last, fallTransition, space, result));
  REQUIRE(result.value == 10.0);
}

TEST_CASE("Sdc Parser: riseTransition", "[parser][SdcParser]")
{
  std::string input = "-input_transition_rise 20.0";
  auto first = input.begin();
  auto last = input.end();
  ast::AttrValue result;
  CHECK(parsePhrase(first, last, riseTransition, space, result));
  REQUIRE(result.value == 20.0);
}
