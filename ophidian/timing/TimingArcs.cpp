#include "TimingArcs.h"

namespace ophidian
{
namespace timing
{

TimingArcs::TimingArcs() :
    mFrom(makeProperty<circuit::Pin>()),
    mTo(makeProperty<circuit::Pin>()),
    mArcNames(makeProperty<std::string>())
{

}

TimingArcs::~TimingArcs()
{

}

Arc TimingArcs::add(const std::string & arcName)
{
    if(mName2Arc.find(arcName) == mName2Arc.end())
    {
        auto arc = mArcs.add();
        mArcNames[arc] = arcName;
        mName2Arc[arcName] = arc;
        return arc;
    }
    else {
        return mName2Arc[arcName];
    }
}

void TimingArcs::from(const Arc & arc, const circuit::Pin & from_)
{
    mFrom[arc] = from_;
}

void TimingArcs::to(const Arc & arc, const circuit::Pin & to_)
{
    mTo[arc] = to_;
}

void TimingArcs::erase(const Arc & arc)
{
    mName2Arc.erase(mArcNames[arc]);
    mArcs.erase(arc);
}

void TimingArcs::reserve(uint32_t size)
{
    mArcs.reserve(size);
}

uint32_t TimingArcs::size() const
{
    return mArcs.size();
}

TimingArcs::ArcsIterator TimingArcs::begin() const
{
    return mArcs.begin();
}

TimingArcs::ArcsIterator TimingArcs::end() const
{
    return mArcs.end();
}

uint32_t TimingArcs::capacity() const
{
    return mArcs.capacity();
}

Arc TimingArcs::find(const std::string & arcName)
{
    return mName2Arc[arcName];
}

ophidian::util::Range<TimingArcs::ArcsIterator> TimingArcs::range() const
{
    return ophidian::util::Range<TimingArcs::ArcsIterator>(mArcs.begin(), mArcs.end());
}

circuit::Pin TimingArcs::from(const Arc & arc) const
{
    return mFrom[arc];
}

circuit::Pin TimingArcs::to(const Arc & arc) const
{
    return mTo[arc];
}

} // namespace timing
} // namespace ophidian
