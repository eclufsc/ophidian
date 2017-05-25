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

#ifndef OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
#define OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H

#include <cassert>

#include "Association.h"

namespace ophidian
{
namespace entity_system
{

template <typename WholeEntity_, typename PartEntity_>
class Composition : public Association<WholeEntity_, PartEntity_>
{
public:
	using Parent = Association<WholeEntity_, PartEntity_>;
	using WholeSystem = typename Parent::WholeSystem;
	using PartSystem = typename Parent::PartSystem;
	using Whole = typename Parent::Whole;
	using Part = typename Parent::Part;

	Composition(const WholeSystem& whole, PartSystem& part) :
		Parent(whole, part)
	{
		Parent::attach(*whole.notifier());
	}

	void erase(const Whole& whole) override
	{
		Part current = Parent::firstPart(whole);
		while (current != Part())
		{
			Part next = Parent::nextPart(current);
			Parent::partSystem_.erase(current);
			current = next;
		}
		Parent::erase(whole);
	}

	void clear() override
	{
		std::vector<Part> toErase;
		std::copy_if(Parent::partSystem_.begin(), Parent::partSystem_.end(),
		             std::back_inserter(toErase), [this](const Part & p) -> bool {
					return (Parent::whole(p) != Whole());
				});
		for(auto const & part : toErase)
		{
			Parent::partSystem_.erase(part);
		}
		Parent::clear();
	}
private:
};

} // namespace entity_system
} // namespace ophidian

#endif // OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
