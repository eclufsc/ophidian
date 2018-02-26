#include "TimingArcs.h"

namespace ophidian
{
namespace timing
{

TimingArcs::TimingArcs() :
    mFrom(makeProperty<standard_cell::Pin>()),
    mTo(makeProperty<standard_cell::Pin>()),
    mArcNames(makeProperty<std::string>())
{

}

TimingArcs::~TimingArcs()
{

}

TimingArc TimingArcs::add(const std::string & arcName)
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

void TimingArcs::from(const TimingArc & arc, const standard_cell::Pin & from_)
{
    mFrom[arc] = from_;
}

void TimingArcs::to(const TimingArc & arc, const standard_cell::Pin & to_)
{
    mTo[arc] = to_;
}

void TimingArcs::erase(const TimingArc & arc)
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

TimingArc TimingArcs::find(const std::string & arcName)
{
    return mName2Arc[arcName];
}

ophidian::util::Range<TimingArcs::ArcsIterator> TimingArcs::range() const
{
    return ophidian::util::Range<TimingArcs::ArcsIterator>(mArcs.begin(), mArcs.end());
}

standard_cell::Pin TimingArcs::from(const TimingArc & arc) const
{
    return mFrom[arc];
}

standard_cell::Pin TimingArcs::to(const TimingArc & arc) const
{
    return mTo[arc];
}

} // namespace timing
} // namespace ophidian
