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

#include "EntitySystem.h"

namespace ophidian {
namespace entity_system {

EntityBase::EntityBase() :
    id_(std::numeric_limits<uint32_t>::max()),
    system_(nullptr)
{

}

EntityBase::~EntityBase()
{

}

bool EntityBase::operator==(const EntityBase &entity) const
{
    return id_ == entity.id_ && system_ == entity.system_;
}

bool EntityBase::operator!=(const EntityBase &entity) const
{
    return !((*this) == entity);
}

EntityBase::EntityBase(uint32_t id, EntitySystemBase * system) :
    id_(id),
    system_(system)
{

}

uint32_t EntitySystemBase::id(const EntityBase &en) const
{
    return en.id_;
}

}
}
