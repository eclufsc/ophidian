#ifndef ASSOCIATION_H
#define ASSOCIATION_H

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

                void shrink() override
                {
                    nextPart_.shrink();
                    whole_.shrink();
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
                        association_.erasePart(whole(item), item);
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

        Association(const WholeSystem& whole, PartSystem& part)  :
            EntitySystem<Whole>::NotifierType::ObserverBase(*whole.notifier()),
            firstPart_(whole),
            part2Whole_(part, *this),
            partSystem_(part),
            numParts_(whole, 0)
        {
            EntitySystem<Whole>::NotifierType::ObserverBase::detach();
        }

        Whole whole(const Part& p) const
        {
            return part2Whole_.whole(p);
        }

        void addPart(const Whole& w, const Part& p)
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

        void erasePart(const Whole& w, const Part& p)
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

        const Parts parts(const Whole& w) const {
            Parts theParts(*this, w);
            return theParts;
        }

        bool empty() const {
            return numParts_.empty();
        }


        Part firstPart(const Whole& w) const {
            return firstPart_[w];
        }

        Part nextPart(const Part& p) const {
            return part2Whole_.nextPart(p);
        }

        uint32_t numParts(const Whole& w) const {
            return numParts_[w];
        }

    protected:

        void detachAllParts()
        {
            std::fill(numParts_.begin(), numParts_.end(), 0);
            std::fill(firstPart_.begin(), firstPart_.end(), Part());
        }

        virtual void shrink() override
        {
            firstPart_.shrink();
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

#endif // ASSOCIATION_H
