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

#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <stdexcept>

namespace ophidian
{
namespace parser
{

class InexistentFile : public std::runtime_error {
public:
	InexistentFile() : std::runtime_error("The given file was not found") { }
	InexistentFile(std::string file) : std::runtime_error("The file \"" + file + "\" was not found") { }
};
} // namespace parser
} // namespace ophidian

#endif
