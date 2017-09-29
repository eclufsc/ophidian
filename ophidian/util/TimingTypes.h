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

#ifndef OPHIDIAN_UTIL_TIMING_TYPES_H
#define OPHIDIAN_UTIL_TIMING_TYPES_H


//! Declaration of basic timing types
namespace ophidian
{
namespace timing
{

enum mode { EARLY_MODE = 0, LATE_MODE = 1};

enum transition {RISE_TRANSITION = 0, FALL_TRANSITION = 1};

} //namespace util
} //namespace ophidian

#endif // OPHIDIAN_UTIL_TIMING_TYPES_H
