#ifndef OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H
#define OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H

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

        ~Aggregation()
        {
            Parent::detach();
        }

        void erase(const Whole& whole) override
        {
            Part current = Parent::firstPart(whole);
            while(current != Part())
            {
                Part next = Parent::nextPart(current);
                Parent::eraseAssociation(whole, current);
                current = next;
            }
            Parent::erase(whole);
        }

        void clear() override
        {
            for(auto const & part : Parent::partSystem_)
            {
                Parent::eraseAssociation(Parent::whole(part), part);
            }
            Parent::clear();
        }
    private:
};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H
