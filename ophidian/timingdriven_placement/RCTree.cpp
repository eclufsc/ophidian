#include "RCTree.h"


namespace ophidian
{
namespace timingdriven_placement
{

RCTree::RCTree() :
    mNames(mGraph),
    mCapacitances(mGraph),
    mResistances(mGraph),
    mLumpedCapacitance(0.0)
{

}

RCTree::RCTree(const RCTree & other):
    mNames(mGraph),
    mCapacitances(mGraph),
    mResistances(mGraph),
    mLumpedCapacitance(0.0)
{
    *this = other;
}

RCTree & RCTree::operator=(const RCTree & other)
{
    lemon::GraphCopy<GraphType, GraphType> cg(other.mGraph, mGraph);
    GraphType::NodeMap<Capacitor> nr(other.mGraph);
    cg.nodeRef(nr);

    GraphType::EdgeMap<Resistor> ecr(mGraph);
    cg.edgeCrossRef(ecr);

    cg.nodeMap(other.mNames, mNames);
    cg.nodeMap(other.mCapacitances, mCapacitances);
    cg.edgeMap(other.mResistances, mResistances);
    mLumpedCapacitance = other.mLumpedCapacitance;
    cg.run();

    for(GraphType::NodeIt it(mGraph); it != lemon::INVALID; ++it)
        mName2Capacitor[mNames[it]] = it;

    for(auto tap : other.mTaps)
        mTaps.push_back(nr[tap]);

    return *this;
}

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
    mCapacitances[newCap] = util::farad_t(0.0);

    return newCap;
}

RCTree::Resistor RCTree::addResistor(const Capacitor & u, const Capacitor & v, const util::ohm_t res)
{
    Resistor resUV;

    if (resistor(u, v) != lemon::INVALID)
        resUV = resistor(u, v);
    else
        resUV = mGraph.addEdge(u, v);

    mResistances[resUV] = res;

    return resUV;
}

void RCTree::insertTap(const Capacitor & cap)
{
    mTaps.push_back(cap);
}

std::string RCTree::name(const Capacitor & cap)
{
    return mNames[cap];
}

RCTree::Capacitor RCTree::capacitor(const std::string name)
{
    return mName2Capacitor[name];
}

RCTree::Capacitor RCTree::oppositeCapacitor(const Capacitor & u, const Resistor & res)
{
    return mGraph.oppositeNode(u, res);
}

RCTree::Resistor RCTree::resistor(const Capacitor & u, const Capacitor & v)
{
    if (!mGraph.valid(u) || !mGraph.valid(v))
        return lemon::INVALID;

    for(GraphType::OutArcIt it(mGraph, u); it != lemon::INVALID; ++it)
        if (mGraph.id(mGraph.oppositeNode(u, it)) == mGraph.id(v))
            return it;

    return lemon::INVALID;
}

RCTree::ResistorIt RCTree::resistors(const Capacitor & cap)
{
    return GraphType::OutArcIt(mGraph, cap);
}

void RCTree::capacitance(const Capacitor & cap, const util::farad_t value)
{
    mLumpedCapacitance -= mCapacitances[cap];
    mCapacitances[cap] = value;
    mLumpedCapacitance += mCapacitances[cap];
}

util::farad_t RCTree::capacitance(const Capacitor & cap)
{
    return mCapacitances[cap];
}

util::ohm_t RCTree::resistance(const Resistor & res)
{
    return mResistances[res];
}

std::size_t RCTree::size(Capacitor)
{
    return lemon::countNodes(mGraph);
}

std::size_t RCTree::size(Resistor)
{
    return lemon::countEdges(mGraph);
}

util::farad_t RCTree::lumped()
{
    return mLumpedCapacitance;
}

const RCTree::GraphType & RCTree::g()
{
    return mGraph;
}

}   // namespace timingdriven_placement
}   // namespace ophidian
