#ifndef COMPOSITION_H
#define COMPOSITION_H

#include "Property.h"
#include <cassert>

namespace ophidian
{
    namespace entity_system
    {
        template <class Composition_>
        class PartEraser
        {
            public:
                PartEraser(Composition_& composition) :
                    composition_(composition)
                {

                }

                void erase(const typename Composition_::Whole& w, const typename Composition_::Part& p)
                {
                    composition_.erasePart(w, p);
                }

            private:
                Composition_& composition_;

        };


        template <class WholeEntity_, class PartEntity_>
        class CompositionBase : public Property<WholeEntity_, std::vector<PartEntity_> >
        {
            protected:
                using Whole = WholeEntity_;
                using Part = PartEntity_;
                using PartContainer = std::vector<Part>;
                using Parent = Property<Whole, PartContainer >;
                using WholeSystem = EntitySystem<Whole>;
                using PartSystem = EntitySystem<Part>;

                void eraseAllUnattachedParts()
                {
                    std::deque<Part> toErase;
                    std::copy_if(partSystem_.begin(), partSystem_.end(),
                                 std::back_inserter(toErase), [this](const Part & p)->bool{ return !(whole(p) == Whole()); });
                    std::for_each(toErase.cbegin(), toErase.cend(), std::bind(&PartSystem::erase, &partSystem_,
                                  std::placeholders::_1));
                }

                void eraseAllParts(const Whole& w)
                {
                    auto const& wholeParts = parts(w);
                    std::for_each(wholeParts.begin(), wholeParts.end(), std::bind(&PartSystem::erase, &partSystem_,
                                  std::placeholders::_1));
                }



                class PartOfComposition : public Property<Part, Whole >
                {
                    public:
                        using Parent = Property<Part, Whole >;
                        PartOfComposition(const EntitySystem<Part>& partSystem, CompositionBase& composition) :
                            Parent(partSystem),
                            composition_(composition)
                        {
                        }

                    private:

                        void erase(const Part& item) override
                        {
                            const Whole& whole = (*this)[item];

                            if (!(whole == Whole()))
                            {
                                composition_.erasePart((*this)[item], item);
                            }

                            Parent::erase(item);
                        }

                        void erase(const std::vector<Part>& parts) override
                        {
                            for (auto const& part : parts)
                            {
                                erase(part);
                            }
                        }

                        void clear() override
                        {
                            composition_.clearAllPartsContainers();
                        }

                        CompositionBase& composition_;
                };

            public:

                CompositionBase(const WholeSystem& whole, PartSystem& part)  :
                    Parent(whole),
                    part2Whole_(part, *this),
                    partSystem_(part)
                {
                    Parent::detach();
                }

                Whole whole(const Part& p) const
                {
                    return part2Whole_[p];
                }

                void addPart(const Whole& w, const Part& p)
                {
                    auto& parts = (*this)[w];
                    assert(std::count(parts.cbegin(), parts.cend(), p) == 0);
                    parts.push_back(p);
                    part2Whole_[p] = w;
                }

                void erasePart(const Whole& w, const Part& p)
                {
                    auto& parts = (*this)[w];
                    assert(std::count(parts.cbegin(), parts.cend(), p) == 1);
                    part2Whole_[p] = Whole();
                    parts.erase(std::find(parts.begin(), parts.end(), p), parts.end());
                }

                void clearParts(const Whole & w)
                {
                    auto& parts = (*this)[w];
                    for(auto const & p : parts)
                    {
                        part2Whole_[p] = Whole();
                    }
                    parts.clear();
                }

                const PartContainer& parts(const Whole& w) const
                {
                    return (*this)[w];
                }


                using Parent::clear;
                using Parent::erase;

            private:
                void clearAllPartsContainers()
                {
                    std::for_each(Parent::begin(), Parent::end(), std::bind(&PartContainer::clear, std::placeholders::_1));
                }

                PartOfComposition part2Whole_;
            protected:
                PartSystem& partSystem_;
        };

        template <typename WholeEntity_, typename PartEntity_>
        class Composition : public CompositionBase<WholeEntity_, PartEntity_>
        {
            public:
                using Parent = CompositionBase<WholeEntity_, PartEntity_>;
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
                    while(!Parent::parts(whole).empty())
                    {
                        auto last_it = Parent::parts(whole).end();
                        --last_it;
                        Parent::partSystem_.erase(*last_it);
                    }
                    Parent::erase(whole);
                }

                void clear() override
                {
                    Parent::eraseAllUnattachedParts();
                    Parent::clear();
                }
            private:
        };




    }
}

#endif // COMPOSITION_H
