#ifndef AGGREGATION_H
#define AGGREGATION_H

#include "Association.h"

namespace ophidian {
namespace entity_system {

template <typename WholeEntity_, typename PartEntity_>
class Aggregation : public Association<WholeEntity_, PartEntity_>
{
    public:
        using Parent = Association<WholeEntity_, PartEntity_>;
        using WholeSystem = typename Parent::WholeSystem;
        using PartSystem = typename Parent::PartSystem;
        using Whole = typename Parent::Whole;
        using Part = typename Parent::Part;

        Aggregation(const WholeSystem& whole, PartSystem& part) :
            Parent(whole, part)
        {
            Parent::attach(*whole.notifier());
        }

        void erase(const Whole& whole) override
        {
            Part current = Parent::firstPart(whole);
            while(current != Part())
            {
                Part next = Parent::nextPart(current);
                Parent::erasePart(whole, current);
                current = next;
            }
            Parent::erase(whole);
        }

        void clear() override
        {

        }
    private:
};

}
}

#endif // AGGREGATION_H
