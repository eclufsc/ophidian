#ifndef COMPOSITION_H
#define COMPOSITION_H

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
                                 std::back_inserter(toErase), [this](const Part & p)->bool{ return (Parent::whole(p) != Whole()); });
                    for(auto const & part : toErase)
                    {
                        Parent::partSystem_.erase(part);
                    }
                    Parent::clear();
                }
            private:
        };

    }
}

#endif // COMPOSITION_H
