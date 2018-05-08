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

RCTree::RCTree(const RCTree & other) :
    mNames(mGraph),
    mCapacitances(mGraph),
    mResistances(mGraph),
    mLumpedCapacitance(0.0),
    mValidPred(false),
    mPred(mGraph)
{
    *this = other;
}

RCTree & RCTree::operator=(const RCTree & other)
{
    lemon::GraphCopy<graph_type, graph_type> cg(other.mGraph, mGraph);
    capacitor_map_type<capacitor_type> nr(other.mGraph);
    cg.nodeRef(nr);

    resistor_map_type<resistor_type> ecr(mGraph);
    cg.edgeCrossRef(ecr);

    cg.nodeMap(other.mNames, mNames);
    cg.nodeMap(other.mCapacitances, mCapacitances);
    cg.edgeMap(other.mResistances, mResistances);
    mLumpedCapacitance = other.mLumpedCapacitance;
    cg.run();

    for(graph_type::NodeIt it(mGraph); it != lemon::INVALID; ++it)
        mName2Capacitor[mNames[it]] = it;

    for(auto tap : other.mTaps)
        mTaps.push_back(nr[tap]);

    return *this;
}

RCTree::~RCTree() {

}

RCTree::capacitor_type RCTree::addCapacitor(const std::string name)
{
    capacitor_type newCap;

    if (mGraph.valid(mName2Capacitor[name]))
        if (mNames[mName2Capacitor[name]] == name)
            return mName2Capacitor[name];
        else
            newCap = mGraph.addNode();
    else
        newCap = mGraph.addNode();

    mName2Capacitor[name] = newCap;
    mNames[newCap] = name;
    mCapacitances[newCap] = capacitance_unit_type(0.0);
    mValidPred = false;

    return newCap;
}

RCTree::resistor_type RCTree::addResistor(const capacitor_type & u, const capacitor_type & v, const resistance_unit_type res)
{
    resistor_type resUV;

    if (resistor(u, v) != lemon::INVALID)
        resUV = resistor(u, v);
    else
        resUV = mGraph.addEdge(u, v);

    mResistances[resUV] = res;
    mValidPred = false;

    return resUV;
}

void RCTree::insertTap(const capacitor_type & cap)
{
    mTaps.push_back(cap);
}

std::string RCTree::name(const capacitor_type & cap) const
{
    if (mGraph.valid(cap))
        return mNames[cap];

    return "Invalid";
}

RCTree::capacitor_type RCTree::capacitor(const std::string name) const
{
    if (mName2Capacitor.find(name) != mName2Capacitor.end())
        return mName2Capacitor.at(name);

    return lemon::INVALID;
}

RCTree::capacitor_type RCTree::oppositeCapacitor(const capacitor_type & u, const resistor_type & res) const
{
    return mGraph.oppositeNode(u, res);
}

RCTree::resistor_type RCTree::resistor(const capacitor_type & u, const capacitor_type & v) const
{
    if (!mGraph.valid(u) || !mGraph.valid(v))
        return lemon::INVALID;

    for(graph_type::OutArcIt it(mGraph, u); it != lemon::INVALID; ++it)
        if (mGraph.id(mGraph.oppositeNode(u, it)) == mGraph.id(v))
            return it;

    return lemon::INVALID;
}

RCTree::resistor_iterator_type RCTree::resistors(const capacitor_type & cap) const
{
    return graph_type::OutArcIt(mGraph, cap);
}

void RCTree::capacitance(const capacitor_type & cap, const capacitance_unit_type value)
{
    mLumpedCapacitance -= mCapacitances[cap];
    mCapacitances[cap] = value;
    mLumpedCapacitance += mCapacitances[cap];
}

RCTree::capacitance_unit_type RCTree::capacitance(const capacitor_type & cap) const
{
    return mCapacitances[cap];
}

RCTree::resistance_unit_type RCTree::resistance(const resistor_type & res) const
{
    return mResistances[res];
}

RCTree::capacitor_type RCTree::pred(const capacitor_type& cap)
{
    if (!mGraph.valid(cap))
        return lemon::INVALID;

    if (!mValidPred)
        topology_updates();

    return mGraph.oppositeNode(cap, mPred[cap]);
}

const RCTree::container_type<RCTree::capacitor_type>& RCTree::order()
{
    if (mValidPred)
        return mOrder;

    if (lemon::connected(mGraph))
        topology_updates();

    if (!mValidPred)
        mOrder.clear();

    return mOrder;
}

std::size_t RCTree::size(capacitor_type) const
{
    return lemon::countNodes(mGraph);
}

std::size_t RCTree::size(resistor_type) const
{
    return lemon::countEdges(mGraph);
}

RCTree::capacitance_unit_type RCTree::lumped() const
{
    return mLumpedCapacitance;
}

void RCTree::source(const capacitor_type & cap)
{
    mSource = cap;
}

RCTree::capacitor_type RCTree::source() const
{
    return mSource;
}

const RCTree::graph_type & RCTree::g() const
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
    std::deque<capacitor_type> ready;
    ready.push_back(mSource);
    mOrder.resize(0);

    while(!ready.empty())
    {
        auto curr_cap = ready.front();
        ready.pop_front();

        stat[curr_cap] = CLOSED;
        mOrder.push_back(curr_cap);

        for(graph_type::OutArcIt it(mGraph, curr_cap); it != lemon::INVALID; ++it)
        {
            capacitor_type target = mGraph.target(it);
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
