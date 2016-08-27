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
            while(!Parent::parts(whole).empty())
            {
                auto last_it = Parent::parts(whole).end();
                --last_it;
                Parent::erasePart(whole, *last_it);
            }
            Parent::erase(whole);
        }

        void clear() override
        {
            for(auto const & part : Parent::partSystem_)
            {
                Parent::erasePart(Parent::whole(part), part);
            }
            Parent::clear();
        }
    private:
};

}
}

#endif // AGGREGATION_H
