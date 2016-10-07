#include "Flute.h"
#include <flute/flute.h>
#include <ophidian/geometry/Distance.h>
#include <ophidian/geometry/Operations.h>

namespace ophidian {
namespace interconnection {

struct Flute::Impl
{
};

Flute::Flute() :
    this_(std::make_unique<Impl>())
{
    readLUT();
}

Flute &Flute::instance()
{
    static Flute instance;
    return instance;
}

Flute::~Flute()
{

}

std::unique_ptr<SteinerTree> Flute::singleSegment(const geometry::Point &p1, const geometry::Point &p2)
{
    auto steiner = std::make_unique<SteinerTree>();
    steiner->add(steiner->add(p1), steiner->add(p2));
    return std::move(steiner);
}

std::unique_ptr<SteinerTree> Flute::trivialSteinerTree(const geometry::Point &p)
{
   auto steiner = std::make_unique<SteinerTree>();
   steiner->add(p);
   return std::move(steiner);
}

std::unique_ptr<SteinerTree> Flute::callFlute(const std::vector<unsigned> &X, const std::vector<unsigned> &Y, const geometry::Point & offset)
{
    Tree tree = flute(static_cast<int32_t>(X.size()), const_cast<unsigned*>(X.data()), const_cast<unsigned*>(Y.data()), ACCURACY);
    auto steiner = std::make_unique<SteinerTree>();
    const int numBranches = 2 * tree.deg - 2;
    geometry::ManhattanDistance distance;
    for(int i = 0; i < numBranches; ++i)
    {
        const Branch & branch = tree.branch[i];
        int n = tree.branch[i].n;
        if(i == n)
        {
            continue;
        }
        const Branch & branchN = tree.branch[n];

        geometry::Point u{static_cast<double>(branch.x), static_cast<double>(branch.y)};
        geometry::Point v{static_cast<double>(branchN.x), static_cast<double>(branchN.y)};

        auto translate = [&offset](geometry::Point& p){
            p.x( p.x() - offset.x() );
            p.y( p.y() - offset.y() );
        };

        translate(u); translate(v);

        if(std::abs(distance(u, v)) > std::numeric_limits<double>::epsilon())
        {
            steiner->add(steiner->add(u), steiner->add(v));
        }
    }
    delete[] tree.branch;
    return std::move(steiner);
}

}
}
