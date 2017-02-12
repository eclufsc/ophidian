#ifndef OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
#define OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H

#include "Property.h"
#include <cassert>

namespace ophidian {
namespace entity_system {

template <class Key, class Value>
class DetachedProperty :  public Property<Key, Value > {
public:
    using Parent = Property<Key, Value >;
    DetachedProperty(const EntitySystem<Key>& system) :
        Parent(system),
        system_(system)
    {
        Parent::detach();
    }

    using Parent::add;
    using Parent::build;
    using Parent::clear;

    virtual void erase(const Key& item) override
    {
        std::swap(Parent::properties_.back(), Parent::properties_[system_.id(item)]);
        Parent::properties_.pop_back();
    }

    const Value& operator [](const Key & k) const {
        return Parent::properties_[system_.id(k)];
    }

    Value& operator [](const Key & k) {
        return Parent::properties_[system_.id(k)];
    }
private:
    const EntitySystem<Key>& system_;
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
                    nextPart_(partSystem),
                    whole_(partSystem),
                    association_(composition)
                {
                }

                Whole whole(const Part & p) const
                {
                    return whole_[p];
                }

                void whole(const Part & p, const Whole & w)
                {
                    whole_[p] = w;
                }

                Part nextPart(const Part &p) const
                {
                    return nextPart_[p];
                }

                void nextPart(const Part& p1, const Part& p2)
                {
                    nextPart_[p1] = p2;
                }


            private:

                void shrinkToFit() override
                {
                    nextPart_.shrinkToFit();
                    whole_.shrinkToFit();
                }

                void reserve(std::uint32_t size) override
                {
                    nextPart_.reserve(size);
                    whole_.reserve(size);
                }

                void add(const Part & item ) override
                {
                    nextPart_.add(item);
                    whole_.add(item);
                }

                void add(const std::vector<Part> & items) override
                {
                    nextPart_.add(items);
                    whole_.add(items);
                }

                void erase(const Part& item) override
                {
                    if(whole(item) != Whole())
                    {
                        association_.eraseAssociation(whole(item), item);
                    }
                    nextPart_.erase(item);
                    whole_.erase(item);
                }

                void clear() override
                {
                    association_.detachAllParts();
                    nextPart_.clear();
                    whole_.clear();
                }

                Association& association_;
                DetachedProperty<Part, Part> nextPart_;
                DetachedProperty<Part, Whole> whole_;
        };

    public:

        //! Parts
        /*!
          A wrapper class to provide a iterable interface (begin(), end()) for the Parts container of a Whole.
        */
        class Parts {
        public:

            class PartIterator : public std::iterator<std::forward_iterator_tag, Part> {
            public:
                PartIterator(const Association * association, const Whole& w) :
                    association_(association),
                    whole_(w),
                    part_(association->firstPart(w))
                {

                }

                PartIterator() :
                    association_(nullptr),
                    whole_(Whole()),
                    part_(Part())
                {

                }

                const Part& operator*() {
                    return part_;
                }
                PartIterator & operator++(void) {
                    part_ = association_->nextPart(part_);
                    return *this;
                }
                PartIterator & operator=(const PartIterator & p)
                {
                    association_ = p.association_;
                    whole_ = p.whole_;
                    return *this;
                }
                bool operator!=(const PartIterator & p) const
                {
                    return part_ != p.part_;
                }
                bool operator==(const PartIterator & p) const
                {
                    return !((*this) != p);
                }
            private:
                const Association * association_;
                Whole whole_;
                Part part_;
            };

            Parts(const Association & association, const Whole & whole) :
                association_(association),
                whole_(whole)
            {

            }

            PartIterator begin() const
            {
                return PartIterator(&association_, whole_);
            }
            PartIterator end() const
            {
                return PartIterator();
            }
            uint32_t size() const
            {
                return association_.numParts(whole_);
            }
            bool empty() const
            {
                return size() == 0;
            }
        private:
            const Association & association_;
            const Whole whole_;
        };

        //! Construct Association
        /*!
          \brief Construct an association between two EntitySystem objects
        */
        Association(const WholeSystem& whole, PartSystem& part)  :
            EntitySystem<Whole>::NotifierType::ObserverBase(*whole.notifier()),
            firstPart_(whole),
            part2Whole_(part, *this),
            partSystem_(part),
            numParts_(whole, 0)
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
            return part2Whole_.whole(p);
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

            part2Whole_.whole(p, w);

            if(first == Whole())
            {
                firstPart_[w] = p;
                numParts_[w] = 1;
                return;
            }

            firstPart_[w] = p;
            part2Whole_.nextPart(p, first);
            ++numParts_[w];

        }

        //! Erase association
        /*!
          \brief Remove the association between Part and Whole entities.
          \param w A handler for the Whole Entity.
          \param p A handler for the Part Entity.
        */
        void eraseAssociation(const Whole& w, const Part& p)
        {
            --numParts_[w];

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
            return numParts_.empty();
        }

        //! First Part of a Whole
        /*!
          \brief Returns the first part of a Whole.
          \param w A handler for the Whole Entity.
          \return A handler for the first Part of \p w.
        */
        Part firstPart(const Whole& w) const {
            return firstPart_[w];
        }

        //! Next Part in an association
        /*!
          \brief Returns the next part of a whole, given the current part.
          \param p A handler for the current part of a Whole
          \return A handler for the next Part in association, after \p w.
          \remarks The association is implemented as a linked list. A Whole entity has a property containing the handler for its first part. Each part has a property containing the next part in association. We assume a part can only be part of one whole at a time.
        */
        Part nextPart(const Part& p) const {
            return part2Whole_.nextPart(p);
        }

        //! Number of parts of a whole
        /*!
          \brief Returns the number of parts of a given whole.
          \param w A handler for the Whole
          \return The number of parts of \p w
        */
        uint32_t numParts(const Whole& w) const {
            return numParts_[w];
        }

    protected:

        void detachAllParts()
        {
            std::fill(numParts_.begin(), numParts_.end(), 0);
            std::fill(firstPart_.begin(), firstPart_.end(), Part());
        }

        virtual void shrinkToFit() override
        {
            firstPart_.shrinkToFit();
        }

        virtual void reserve(uint32_t size) override
        {
            firstPart_.reserve(size);
        }

        virtual void add(const Whole & item ) override
        {
            firstPart_.add(item);
        }

        virtual void add(const std::vector<Whole> & items) override
        {
            firstPart_.add(items);
        }

        virtual void erase(const Whole& item) override
        {
            firstPart_.erase(item);
        }

        virtual void clear() override
        {
            firstPart_.clear();
        }

        PartOfComposition part2Whole_;
        DetachedProperty<Whole, Part> firstPart_;
        Property<Whole, uint32_t> numParts_;
        PartSystem& partSystem_;

private:
        void whole(const Part& p, const Whole& w)
        {
            part2Whole_.whole(p, w);
        }

        void firstPart(const Whole & w, const Part & p) {
            firstPart_[w] = p;
        }

        void nextPart(const Part& p1, const Part &p2) {
            part2Whole_.nextPart(p1, p2);
        }


};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
