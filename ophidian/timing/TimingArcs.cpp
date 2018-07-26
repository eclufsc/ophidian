#include "TimingArcs.h"

namespace ophidian
{
namespace timing
{

TimingArcs::TimingArcs(const standard_cells_type & stdCells) :
    mFrom(makeProperty<standard_pin_entity_type>()),
    mTo(makeProperty<standard_pin_entity_type>()),
    mArcNames(makeProperty<timing_arc_name_type>()),
    mPin2TimingArcs(stdCells.makeProperty<timing_arc_vector_type>(standard_pin_entity_type()))
{
}

TimingArcs::timing_arc_entity_type TimingArcs::add(const timing_arc_name_type & arcName)
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

void TimingArcs::from(const timing_arc_entity_type & arc, const standard_pin_entity_type & from_)
{
    mFrom[arc] = from_;
    mPin2TimingArcs[from_].push_back(arc);
}

void TimingArcs::to(const timing_arc_entity_type & arc, const standard_pin_entity_type & to_)
{
    mTo[arc] = to_;
    mPin2TimingArcs[to_].push_back(arc);
}

void TimingArcs::erase(const timing_arc_entity_type & arc)
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

TimingArcs::timing_arc_entity_type TimingArcs::find(const timing_arc_name_type & arcName)
{
    return mName2Arc[arcName];
}

TimingArcs::timing_arc_name_type TimingArcs::name(const timing_arc_entity_type & arc)
{
    return mArcNames[arc];
}

TimingArcs::timing_arc_range_type TimingArcs::range() const
{
    return timing_arc_range_type(mArcs.begin(), mArcs.end());
}

TimingArcs::standard_pin_entity_type TimingArcs::from(const timing_arc_entity_type & arc) const
{
    return mFrom[arc];
}

TimingArcs::standard_pin_entity_type TimingArcs::to(const timing_arc_entity_type & arc) const
{
    return mTo[arc];
}

const TimingArcs::timing_arc_vector_type & TimingArcs::pinArcs(const standard_pin_entity_type & pin) const
{
    return mPin2TimingArcs[pin];
}

} // namespace timing
} // namespace ophidian
