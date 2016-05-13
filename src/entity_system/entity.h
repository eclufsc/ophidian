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

#ifndef OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_H
#define OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_H

#include <limits>
#include <cstdint>
#include <boost/serialization/strong_typedef.hpp>

namespace ophidian{
namespace entity_system{

//I adapted the boost strong typed to ensure a default initialization with std::numeric_limits<T>::max()
#define ENTITY_SYSTEM_STRONG_TYPEDEF(T, D)                              \
struct D                                                        \
    : boost::totally_ordered1< D                                \
    , boost::totally_ordered2< D, T                             \
    > >                                                         \
{                                                               \
    T t;                                                        \
    explicit D(const T t_) : t(t_) {};                          \
    D(): t(std::numeric_limits<T>::max()) {};                                                \
    D(const D & t_) : t(t_.t){}                                 \
    D & operator=(const D & rhs) { t = rhs.t; return *this;}    \
    D & operator=(const T & rhs) { t = rhs; return *this;}      \
    operator const T & () const {return t; }                    \
    operator T & () { return t; }                               \
    bool operator==(const D & rhs) const { return t == rhs.t; } \
    bool operator<(const D & rhs) const { return t < rhs.t; }   \
};

ENTITY_SYSTEM_STRONG_TYPEDEF(uint32_t, entity)
ENTITY_SYSTEM_STRONG_TYPEDEF(uint32_t, entity_index)

static const entity_index invalid_entity_index = static_cast<entity_index>(std::numeric_limits<uint32_t>::max());
static const entity invalid_entity = static_cast<entity>(std::numeric_limits<uint32_t>::max());

} /* namespace entity system */
} /* namespace ophidian */

//adding to std namespace hash operators for entity and pair of entities
namespace std
{
template<> struct hash<ophidian::entity_system::entity>
{
    std::size_t operator()(const ophidian::entity_system::entity & e) const{
        return std::hash<std::uint32_t>()(e);
    }
};

template<> struct hash<std::pair<ophidian::entity_system::entity,ophidian::entity_system::entity>>
{
    std::size_t operator () (const std::pair<ophidian::entity_system::entity,ophidian::entity_system::entity> &p) const {
        return std::hash<uint32_t>{}(p.first) ^ std::hash<uint32_t>{}(p.second);
}
};
}

#endif //OPHIDIAN_SRC_ENTITY_SYSTEM_ENTITY_H
