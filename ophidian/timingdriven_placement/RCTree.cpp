#include "RCTree.h"


namespace ophidian
{
namespace timingdriven_placement
{

RCTree::RCTree() :
    mNames(mGraph),
    mCapacitances(mGraph),
    mResistances(mGraph),
    mLumpedCapacitance(0.0),
    mValidPred(false),
    mPred(mGraph),
    mSource(lemon::INVALID)
{

}

//RCTree::RCTree(const RCTree & other) :
//    mNames(mGraph),
//    mCapacitances(mGraph),
//    mResistances(mGraph),
//    mLumpedCapacitance(0.0),
//    mValidPred(false),
//    mPred(mGraph)
//{
//    *this = other;
//}

//RCTree & RCTree::operator=(const RCTree & other)
//{
//    lemon::GraphCopy<GraphType, GraphType> cg(other.mGraph, mGraph);
//    capacitor_map_type<Capacitor> nr(other.mGraph);
//    cg.nodeRef(nr);

//    resistor_map_type<Resistor> ecr(mGraph);
//    cg.edgeCrossRef(ecr);

//    cg.nodeMap(other.mNames, mNames);
//    cg.nodeMap(other.mCapacitances, mCapacitances);
//    cg.edgeMap(other.mResistances, mResistances);
//    mLumpedCapacitance = other.mLumpedCapacitance;
//    cg.run();

//    for(GraphType::NodeIt it(mGraph); it != lemon::INVALID; ++it)
//        mName2Capacitor[mNames[it]] = it;

//    for(auto tap : other.mTaps)
//        mTaps.push_back(nr[tap]);

//    return *this;
//}

RCTree::~RCTree() {

}

RCTree::Capacitor RCTree::addCapacitor(const std::string name)
{
    Capacitor newCap;

    if (mGraph.valid(mName2Capacitor[name]))
        if (mNames[mName2Capacitor[name]] == name)
            return mName2Capacitor[name];
        else
            newCap = mGraph.addNode();
    else
        newCap = mGraph.addNode();

    mName2Capacitor[name] = newCap;
    mNames[newCap] = name;
    mCapacitances[newCap] = capacitance_type(0.0);
    mValidPred = false;

    return newCap;
}

RCTree::Resistor RCTree::addResistor(const Capacitor & u, const Capacitor & v, const resistance_type res)
{
    Resistor resUV;

    if (resistor(u, v) != lemon::INVALID)
        resUV = resistor(u, v);
    else
        resUV = mGraph.addArc(u, v);

    mResistances[resUV] = res;
    mValidPred = false;

    return resUV;
}

void RCTree::insertTap(const Capacitor & cap)
{
    mTaps.push_back(cap);
}

std::string RCTree::name(const Capacitor & cap) const
{
    if (mGraph.valid(cap))
        return mNames[cap];

    return "Invalid";
}

RCTree::Capacitor RCTree::capacitor(const std::string name)
{
    if (mName2Capacitor.find(name) != mName2Capacitor.end())
        return mName2Capacitor[name];

    return lemon::INVALID;
}

RCTree::Capacitor RCTree::oppositeCapacitor(const Capacitor & u, const Resistor & res) const
{
    return mGraph.oppositeNode(u, res);
}

RCTree::Resistor RCTree::resistor(const Capacitor & u, const Capacitor & v) const
{
    if (!mGraph.valid(u) || !mGraph.valid(v))
        return lemon::INVALID;

    for(GraphType::OutArcIt it(mGraph, u); it != lemon::INVALID; ++it)
        if (mGraph.id(mGraph.oppositeNode(u, it)) == mGraph.id(v))
            return it;

    return lemon::INVALID;
}

RCTree::ResistorIt RCTree::resistors(const Capacitor & cap) const
{
    return GraphType::OutArcIt(mGraph, cap);
}

void RCTree::capacitance(const Capacitor & cap, const capacitance_type value)
{
    mLumpedCapacitance -= mCapacitances[cap];
    mCapacitances[cap] = value;
    mLumpedCapacitance += mCapacitances[cap];
}

RCTree::capacitance_type RCTree::capacitance(const Capacitor & cap) const
{
    return mCapacitances[cap];
}

RCTree::resistance_type RCTree::resistance(const Resistor & res) const
{
    return mResistances[res];
}

RCTree::Capacitor RCTree::pred(const Capacitor& cap)
{
    if (!mGraph.valid(cap))
        return lemon::INVALID;

    if (!mValidPred)
        topology_updates();

    return mGraph.oppositeNode(cap, mPred[cap]);
}

const RCTree::container_type<RCTree::Capacitor>& RCTree::order()
{
    if (mValidPred)
        return mOrder;

    if (lemon::dag(mGraph))
        topology_updates();

    if (!mValidPred)
        mOrder.clear();

    return mOrder;
}

std::size_t RCTree::size(Capacitor) const
{
    return lemon::countNodes(mGraph);
}

std::size_t RCTree::size(Resistor) const
{
    return lemon::countArcs(mGraph);
}

RCTree::capacitance_type RCTree::lumped() const
{
    return mLumpedCapacitance;
}

void RCTree::source(const Capacitor & cap)
{
    mSource = cap;
}

RCTree::Capacitor RCTree::source() const
{
    return mSource;
}

const RCTree::GraphType & RCTree::g() const
{
    return mGraph;
}

void RCTree::topology_updates()
{
    if(source() == lemon::INVALID)
        return;

    enum status : int {
        OPEN, TOUCHED, CLOSED
    };

    capacitor_map_type<status> stat(mGraph, OPEN);
    std::deque<Capacitor> ready;
    ready.push_back(mSource);
    mOrder.resize(0);

    while(!ready.empty())
    {
        auto curr_cap = ready.front();
        ready.pop_front();

        stat[curr_cap] = CLOSED;
        mOrder.push_back(curr_cap);

        for(GraphType::OutArcIt it(mGraph, curr_cap); it != lemon::INVALID; ++it)
        {
            Capacitor target = mGraph.target(it);
            switch(stat[target])
            {
            case OPEN:
                mPred[target] = it;
                stat[target] = TOUCHED;
                ready.push_back(target);
                break;
            case TOUCHED:
            case CLOSED:
                break;
            }
        }
    }

    mValidPred = true;
}

}   // namespace timingdriven_placement
}   // namespace ophidian
