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

#ifndef OPHIDIAN_SDC_PARSER_H
#define OPHIDIAN_SDC_PARSER_H

#include <ophidian/parser/SdcParser.h>

using namespace ophidian::parser;

template <class Iterator, class Rule, class Attribute>
bool parseAttribute(Iterator& first, Iterator & last, const Rule & rule, Attribute & attribute)
{
  return boost::spirit::x3::parse(first, last, rule, attribute) && (first == last);
}

template <class Iterator, class Rule, class Attribute, class Skipper>
bool parsePhrase(Iterator& first, Iterator & last, const Rule & rule, Skipper & skipper, Attribute & attribute)
{
  return boost::spirit::x3::phrase_parse(first, last, rule, skipper, attribute) && (first == last);
}

#endif //OPHIDIAN_SDC_PARSER_H
