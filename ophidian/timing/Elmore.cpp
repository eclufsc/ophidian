#include "Elmore.h"


namespace ophidian
{
namespace timing
{

Elmore::Elmore(const timingdriven_placement::RCTree & tree, const CapacitorRCTree & source) :
    mTree(tree),
    mElmoreDelay(tree.g()),
    mDownstreamCapacitance(tree.g()),
    mPred(tree.g()),
    mSource(source)
{
    std::deque<CapacitorRCTree> toProcess;
    std::set<CapacitorRCTree> reached;
    mPred[mSource] = std::make_pair(lemon::INVALID, lemon::INVALID);
    reached.insert(mSource);
    toProcess.push_back(mSource);
    while (!toProcess.empty())
    {
        auto element = toProcess.front();
        toProcess.pop_front();
        mOrder.push_back(element);
        for (auto it = mTree.resistors(element); it != mTree.invalid(); ++it)
        {
            auto other = mTree.oppositeCapacitor(element, it);
            if (reached.insert(other).second)
            {
                mPred[other] = std::make_pair(element, it);
                toProcess.push_back(other);
            }
        }
    }

    assert(mOrder.size() == mTree.size(CapacitorRCTree()));

    update();
}

Elmore::~Elmore()
{

}

void Elmore::update()
{
    for (auto c : mOrder)
        mDownstreamCapacitance[c] = mTree.capacitance(c);

    for (auto it = mOrder.rbegin(); it != mOrder.rend(); ++it)
        if (mPred[*it].first != lemon::INVALID)
            mDownstreamCapacitance[mPred[*it].first] += mDownstreamCapacitance[*it];

    mElmoreDelay[mSource] = util::second_t(0.0);

    for (auto c : mOrder)
        if (mPred[c].first != lemon::INVALID)
            mElmoreDelay[c] = mElmoreDelay[mPred[c].first] + mTree.resistance(mPred[c].second) * mDownstreamCapacitance[c];
}

util::second_t Elmore::at(const CapacitorRCTree cap) const
{
    return mElmoreDelay[cap];
}

const Elmore::GraphRCTreeType::NodeMap<std::pair<Elmore::CapacitorRCTree, Elmore::ResistorRCTree>> & Elmore::pred() const
{
    return mPred;
}

const std::vector<Elmore::CapacitorRCTree> & Elmore::order() const
{
    return mOrder;
}

}   // namespace timing
}   // namespace ophidian
