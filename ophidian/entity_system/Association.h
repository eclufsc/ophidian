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

#ifndef OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
#define OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H

#include "Property.h"
#include <cassert>

namespace ophidian
{
namespace entity_system
{

template <class Key, class Value>
class DetachedProperty :  public Property<Key, Value >
{
public:
	using Parent = Property<Key, Value >;
	DetachedProperty(const EntitySystem<Key>& system) :
		Parent(system),
        mSystem(system)
	{
		Parent::detach();
	}

	using Parent::add;
	using Parent::build;
	using Parent::clear;

	virtual void erase(const Key& item) override
	{
        std::swap(Parent::mProperties.back(), Parent::mProperties[mSystem.id(item)]);
        Parent::mProperties.pop_back();
	}

	const Value& operator [](const Key & k) const {
        return Parent::mProperties[mSystem.id(k)];
	}

	Value& operator [](const Key & k) {
        return Parent::mProperties[mSystem.id(k)];
	}
private:
    const EntitySystem<Key>& mSystem;
};

//! Association
/*!
   An abstract property to model 1-N associations between entities.
   The "1" side is called Whole. The "N" side is called Part.
 */
template <class WholeEntity_, class PartEntity_>
class Association :
	public EntitySystem<WholeEntity_>::NotifierType::ObserverBase
{
public:
	using Whole = WholeEntity_;
	using Part = PartEntity_;
	using WholeSystem = EntitySystem<Whole>;
	using PartSystem = EntitySystem<Part>;

	class PartOfComposition :
		public EntitySystem<Part>::NotifierType::ObserverBase
	{
public:
		PartOfComposition(const EntitySystem<Part>& partSystem, Association& composition) :
			EntitySystem<Part>::NotifierType::ObserverBase(*partSystem.notifier()),
            mNextPart(partSystem),
            mWhole(partSystem),
            mAssociation(composition)
		{
		}

		Whole whole(const Part & p) const
		{
            return mWhole[p];
		}

		void whole(const Part & p, const Whole & w)
		{
            mWhole[p] = w;
		}

		Part nextPart(const Part &p) const
		{
            return mNextPart[p];
		}

		void nextPart(const Part& p1, const Part& p2)
		{
            mNextPart[p1] = p2;
		}


private:

		void shrinkToFit() override
		{
            mNextPart.shrinkToFit();
            mWhole.shrinkToFit();
		}

		void reserve(std::uint32_t size) override
		{
            mNextPart.reserve(size);
            mWhole.reserve(size);
		}

		void add(const Part & item ) override
		{
            mNextPart.add(item);
            mWhole.add(item);
		}

		void add(const std::vector<Part> & items) override
		{
            mNextPart.add(items);
            mWhole.add(items);
		}

		void erase(const Part& item) override
		{
			if(whole(item) != Whole())
			{
                mAssociation.eraseAssociation(whole(item), item);
			}
            mNextPart.erase(item);
            mWhole.erase(item);
		}

		void clear() override
		{
            mAssociation.detachAllParts();
            mNextPart.clear();
            mWhole.clear();
		}

        Association& mAssociation;
        DetachedProperty<Part, Part> mNextPart;
        DetachedProperty<Part, Whole> mWhole;
	};

public:

	//! Parts
	/*!
	   A wrapper class to provide a iterable interface (begin(), end()) for the Parts container of a Whole.
	 */
	class Parts
	{
public:

		class PartIterator : public std::iterator<std::forward_iterator_tag, Part>
		{
public:
			PartIterator(const Association * association, const Whole& w) :
                mAssociation(association),
                mWhole(w),
                mPart(association->firstPart(w))
			{

			}

			PartIterator() :
                mAssociation(nullptr),
                mWhole(Whole()),
                mPart(Part())
			{

			}

			const Part& operator*() {
                return mPart;
			}
			PartIterator & operator++(void) {
                mPart = mAssociation->nextPart(mPart);
				return *this;
			}
			PartIterator & operator=(const PartIterator & p)
			{
                mAssociation = p.mAssociation;
                mWhole = p.mWhole;
				return *this;
			}
			bool operator!=(const PartIterator & p) const
			{
                return mPart != p.mPart;
			}
			bool operator==(const PartIterator & p) const
			{
				return !((*this) != p);
			}
private:
            const Association * mAssociation;
            Whole mWhole;
            Part mPart;
		};

		Parts(const Association & association, const Whole & whole) :
            mAssociation(association),
            mWhole(whole)
		{

		}

		PartIterator begin() const
		{
            return PartIterator(&mAssociation, mWhole);
		}
		PartIterator end() const
		{
			return PartIterator();
		}
		uint32_t size() const
		{
            return mAssociation.numParts(mWhole);
		}
		bool empty() const
		{
			return size() == 0;
		}
private:
        const Association & mAssociation;
        const Whole mWhole;
	};

	//! Construct Association
	/*!
	   \brief Construct an association between two EntitySystem objects
	 */
	Association(const WholeSystem& whole, PartSystem& part)  :
		EntitySystem<Whole>::NotifierType::ObserverBase(*whole.notifier()),
        mFirstPart(whole),
        mPart2Whole(part, *this),
        mPartSystem(part),
        mNumParts(whole, 0)
	{
		EntitySystem<Whole>::NotifierType::ObserverBase::detach();
	}

	//! Whole of a Part
	/*!
	   \brief Returns the Whole of a Part.
	   \param p A handler for the part we want the Whole.
	   \return A handler for the Whole of \p part
	 */
	Whole whole(const Part& p) const
	{
        return mPart2Whole.whole(p);
	}

	//! Add association
	/*!
	   \brief Makes a Part Entity part of a Whole Entity.
	   \param w A handler for the Whole Entity.
	   \param p A handler for the Part Entity.
	 */
	void addAssociation(const Whole& w, const Part& p)
	{
		auto first = firstPart(w);

        mPart2Whole.whole(p, w);

		if(first == Whole())
		{
            mFirstPart[w] = p;
            mNumParts[w] = 1;
			return;
		}

        mFirstPart[w] = p;
        mPart2Whole.nextPart(p, first);
        ++mNumParts[w];

	}

	//! Erase association
	/*!
	   \brief Remove the association between Part and Whole entities.
	   \param w A handler for the Whole Entity.
	   \param p A handler for the Part Entity.
	 */
	void eraseAssociation(const Whole& w, const Part& p)
	{
        --mNumParts[w];

		whole(p, Whole());


		if(p == firstPart(w))
		{
			firstPart(w, nextPart(p));
			return;
		}

		Part prev = firstPart(w);
		Part curr = nextPart(firstPart(w));
		while(curr != p && curr != Part())
		{
			prev = curr;
			curr = nextPart(curr);
		}

		if(curr == p)
		{
			nextPart(prev, nextPart(curr));
		}

	}

	//! Get the parts of a whole
	/*!
	   \brief Returns a container-like object for the parts of a given whole.
	   \param w A handler for the Whole Entity.
	   \return A Parts object that contains begin(), end() and size() methods.
	 */
	const Parts parts(const Whole& w) const {
		Parts theParts(*this, w);
		return theParts;
	}

	//! Empty Association
	/*!
	   \brief Returns a boolean indicating if the Association is empty (i.e., the whole EntitySystem is empty) or not.
	   \return true if the Whole EntitySystem is empty, false otherwise.
	 */
	bool empty() const {
        return mNumParts.empty();
	}

	//! First Part of a Whole
	/*!
	   \brief Returns the first part of a Whole.
	   \param w A handler for the Whole Entity.
	   \return A handler for the first Part of \p w.
	 */
	Part firstPart(const Whole& w) const {
        return mFirstPart[w];
	}

	//! Next Part in an association
	/*!
	   \brief Returns the next part of a whole, given the current part.
	   \param p A handler for the current part of a Whole
	   \return A handler for the next Part in association, after \p w.
	   \remarks The association is implemented as a linked list. A Whole entity has a property containing the handler for its first part. Each part has a property containing the next part in association. We assume a part can only be part of one whole at a time.
	 */
	Part nextPart(const Part& p) const {
        return mPart2Whole.nextPart(p);
	}

	//! Number of parts of a whole
	/*!
	   \brief Returns the number of parts of a given whole.
	   \param w A handler for the Whole
	   \return The number of parts of \p w
	 */
	uint32_t numParts(const Whole& w) const {
        return mNumParts[w];
	}

protected:

	void detachAllParts()
	{
        std::fill(mNumParts.begin(), mNumParts.end(), 0);
        std::fill(mFirstPart.begin(), mFirstPart.end(), Part());
	}

	virtual void shrinkToFit() override
	{
        mFirstPart.shrinkToFit();
	}

	virtual void reserve(uint32_t size) override
	{
        mFirstPart.reserve(size);
	}

	virtual void add(const Whole & item ) override
	{
        mFirstPart.add(item);
	}

	virtual void add(const std::vector<Whole> & items) override
	{
        mFirstPart.add(items);
	}

	virtual void erase(const Whole& item) override
	{
        mFirstPart.erase(item);
	}

	virtual void clear() override
	{
        mFirstPart.clear();
	}

    PartOfComposition mPart2Whole;
    DetachedProperty<Whole, Part> mFirstPart;
    Property<Whole, uint32_t> mNumParts;
    PartSystem& mPartSystem;

private:
	void whole(const Part& p, const Whole& w)
	{
        mPart2Whole.whole(p, w);
	}

	void firstPart(const Whole & w, const Part & p) {
        mFirstPart[w] = p;
	}

	void nextPart(const Part& p1, const Part &p2) {
        mPart2Whole.nextPart(p1, p2);
	}


};

} // namespace entity_system
} // namespace ophidian

#endif // OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
