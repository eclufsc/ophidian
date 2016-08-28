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
                using PartContainer = typename Parent::PartContainer;

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
