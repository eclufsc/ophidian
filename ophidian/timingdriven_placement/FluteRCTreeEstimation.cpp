#include "FluteRCTreeEstimation.h"

#include <flute.h>

namespace ophidian
{
namespace timingdriven_placement
{

namespace FluteRTree
{
typedef std::pair<geometry::Point, circuit::Pin> RTreeNode;

class RTreeNodeComparator {
public:
    bool operator()(const RTreeNode & node1, const RTreeNode & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<RTreeNode, boost::geometry::index::rstar<16>, boost::geometry::index::indexable<RTreeNode>, RTreeNodeComparator> RTree;
}

FluteRCTreeBuilder::FluteRCTreeBuilder(double maxSegmentLength)
{
    readLUT();
    mParameters.mMaxSegmentLength = maxSegmentLength;
    mParameters.mCapacitancePerMicron = util::farad_t(1.6e-16);
    mParameters.mResistancePerMicron = util::ohm_t(2.535);
}

FluteRCTreeBuilder::~FluteRCTreeBuilder()
{

}

void FluteRCTreeBuilder::maxSegmentLength(double maximum)
{
    mParameters.mMaxSegmentLength = maximum;
}

void FluteRCTreeBuilder::resistancePerMicrons(const util::ohm_t resistance)
{
    mParameters.mResistancePerMicron = resistance;
}

void FluteRCTreeBuilder::capacitancePerMicron(const util::farad_t capacitance)
{
    mParameters.mCapacitancePerMicron = capacitance;
}

FluteRCTreeBuilder::SourceRCTree FluteRCTreeBuilder::build(const placement::Placement & placement,
                                                           const placement::PlacementMapping & placementMap,
                                                           const circuit::LibraryMapping & libraryMapping,
                                                           const circuit::Netlist & netlist,
                                                           const timing::Library & library,
                                                           const parser::Lef & lef,
                                                           const circuit::Net net,
                                                           RCTree & rctree,
                                                           const circuit::Pin source)
{
    auto netPins = netlist.pins(net);
    RCTree::capacitor_type sourceCap;

    auto get_location = [&](const circuit::Pin & pin)
    {
        auto input = netlist.input(pin);
        if (input != circuit::Input())
            return placement.inputPadLocation(input);

        auto output = netlist.output(pin);
        if (output != circuit::Output())
            return placement.outputPadLocation(output);

        return placementMap.location(pin);
    };

    std::set<RCTree::capacitor_type> taps;

    if(netPins.size() == 1)
    {
        auto pinU = *netPins.begin();
        auto capU = rctree.addCapacitor("C0");
        auto tapU = sourceCap = rctree.addCapacitor(netlist.name(pinU));

        taps.insert(tapU);

        rctree.capacitance(tapU, library.capacitance(libraryMapping.pinStdCell(pinU)));
        rctree.addResistor(capU, tapU, util::ohm_t(0.0));

        for(auto t : taps)
            rctree.insertTap(t);

        rctree.source(sourceCap);
        return sourceCap;
    }


    if(netPins.size() == 2)
    {
        auto pinU = *netPins.begin();
        auto pinV = *(++netPins.begin());
        auto positionPinU = get_location(pinU);
        auto positionPinV = get_location(pinV);

        const auto capU = rctree.addCapacitor("C0");
        double length = geometry::ManhattanDistance()(positionPinU.toPoint(), positionPinV.toPoint())
                        / lef.databaseUnits();

        if(length > 0)
        {
            int numberOfSlicedSegments = std::ceil(length / mParameters.mMaxSegmentLength);

            const auto capV = rctree.addCapacitor("C1");
            auto previous = capU;
            double remaining = length;

            for(int j = 0; j < numberOfSlicedSegments; ++j)
            {
                double localLength = std::min(remaining, mParameters.mMaxSegmentLength);

                auto previousCap = rctree.capacitance(previous)
                                 + (localLength / 2.0) * mParameters.mCapacitancePerMicron;
                rctree.capacitance(previous, previousCap);

                RCTree::capacitor_type next;
                if(j != numberOfSlicedSegments-1)
                    next = rctree.addCapacitor("C_0_" + std::to_string(j));
                else
                    next = capV;

                auto nextCap = rctree.capacitance(next)
                             + (localLength / 2.0) * mParameters.mCapacitancePerMicron;
                rctree.capacitance(next, nextCap);
                rctree.addResistor(previous, next, (localLength) * mParameters.mResistancePerMicron);

                previous = next;
                remaining -= localLength;
            }

            RCTree::capacitor_type tapV = sourceCap = rctree.addCapacitor(netlist.name(pinV));
            taps.insert(tapV);
            auto cap_test = library.capacitance(libraryMapping.pinStdCell(pinV));
            rctree.capacitance(tapV, cap_test);
            rctree.addResistor(capV, tapV, util::ohm_t(0.0));
        }
        else
        {
            RCTree::capacitor_type tapV = sourceCap = rctree.addCapacitor(netlist.name(pinV));
            taps.insert(tapV);
            rctree.capacitance(tapV, library.capacitance(libraryMapping.pinStdCell(pinV)));
            rctree.addResistor(capU, tapV, util::ohm_t(0.0));
        }

        RCTree::capacitor_type tapU = rctree.addCapacitor(netlist.name(pinU));
        sourceCap = pinU == source? tapU : sourceCap;
        taps.insert(tapU);
        rctree.capacitance(tapU, library.capacitance(libraryMapping.pinStdCell(pinU)));
        rctree.addResistor(capU, tapU, util::ohm_t(0.0));

        for(auto t : taps)
            rctree.insertTap(t);

        rctree.source(sourceCap);
        return sourceCap;
    }

    std::vector<unsigned> X(netPins.size());
    std::vector<unsigned> Y(netPins.size());
    X.resize(0);
    Y.resize(0);

    for (auto p : netPins)
    {
        util::LocationDbu position = get_location(p);
        X.push_back(static_cast<unsigned>(position.x()));
        Y.push_back(static_cast<unsigned>(position.y()));
    }

    auto fluteTree = flute(netPins.size(), X.data(), Y.data(), ACCURACY);
    std::size_t numBranches = 2 * fluteTree.deg - 2;

    FluteRTree::RTree tapIndexing;
    std::vector<FluteRTree::RTreeNode> nodes(netPins.size());
    nodes.resize(0);

    int i = 0;
    for (auto p : netPins)
    {
        nodes.push_back( { geometry::Point(X[i], Y[i]), p });
        i++;
    }

    tapIndexing.insert(nodes.begin(), nodes.end());

    std::vector<bool> tapCreated(numBranches, false);
    std::map<std::pair<unsigned, unsigned>, int> branchMap;

    for (std::size_t i = 0; i < numBranches; ++i)
    {
        std::size_t n = static_cast<std::size_t>(fluteTree.branch[i].n);

        if (n == i)
            continue;

        int fromI = i;
        int toI = n;

        geometry::Point from(static_cast<double>(fluteTree.branch[i].x), static_cast<double>(fluteTree.branch[i].y));
        geometry::Point to  (static_cast<double>(fluteTree.branch[n].x), static_cast<double>(fluteTree.branch[n].y));

        double length = geometry::ManhattanDistance()(from, to) / lef.databaseUnits();

        std::string fromName = "C_" + std::to_string(i);
        auto fromPositionPair = std::make_pair(fluteTree.branch[i].x, fluteTree.branch[i].y);
        auto cached = branchMap.find(fromPositionPair);

        if(cached != branchMap.end())
        {
            fromI = cached->second;
            fromName = "C_" + std::to_string(fromI);
        }
        else
            branchMap[fromPositionPair] = i;

        std::string toName = "C_" + std::to_string(n);
        auto toPositionPair = std::make_pair(fluteTree.branch[n].x, fluteTree.branch[n].y);
        cached = branchMap.find(toPositionPair);

        if(cached != branchMap.end())
        {
            toI = cached->second;
            toName = "C_" + std::to_string(toI);
        }
        else
            branchMap[toPositionPair] = n;

        const RCTree::capacitor_type capFrom = rctree.addCapacitor(fromName);

        if(fromI != toI)
        {
            const auto capTo = rctree.addCapacitor(toName);
            int numPIs = std::ceil(length / mParameters.mMaxSegmentLength);
            auto previous = capFrom;
            double remaining = length;

            for(int j = 0; j < numPIs; ++j)
            {
                double localLength = std::min(remaining, mParameters.mMaxSegmentLength);

                rctree.capacitance(previous, rctree.capacitance(previous) + (localLength / 2.0) * mParameters.mCapacitancePerMicron);

                RCTree::capacitor_type next;
                if(j != numPIs-1)
                    next = rctree.addCapacitor(fromName + "_" + std::to_string(j));
                else
                    next = capTo;

                rctree.capacitance(next, rctree.capacitance(next) + (localLength / 2.0) * mParameters.mCapacitancePerMicron);
                rctree.addResistor(previous, next, (localLength) * mParameters.mResistancePerMicron);

                previous = next;
                remaining -= localLength;
            }


            std::vector<FluteRTree::RTreeNode> nearest;
            tapIndexing.query(boost::geometry::index::nearest(to, 1), std::back_inserter(nearest));

            bool toIsTap = !nearest.empty() && boost::geometry::equals(to, nearest.front().first);
            if (toIsTap && !tapCreated[toI])
            {
                circuit::Pin pin = nearest.front().second;

                RCTree::capacitor_type tapCap = rctree.addCapacitor(netlist.name(pin));

                rctree.insertTap(tapCap);
                rctree.capacitance(tapCap, library.capacitance(libraryMapping.pinStdCell(pin)));
                rctree.addResistor(capTo, tapCap, util::ohm_t(0.0));

                tapCreated[toI]  = true;
                sourceCap = pin == source? tapCap : sourceCap;
            }
        }


        std::vector<FluteRTree::RTreeNode> nearest;
        tapIndexing.query(boost::geometry::index::nearest(from, 1), std::back_inserter(nearest));

        bool fromIsTap = !nearest.empty() && boost::geometry::equals(from, nearest.front().first);

        if (fromIsTap && !tapCreated[fromI])
        {
            auto pin = nearest.front().second;
            auto tapCap = rctree.addCapacitor(netlist.name(pin));

            rctree.insertTap(tapCap);
            rctree.capacitance(tapCap, library.capacitance(libraryMapping.pinStdCell(pin)));
            rctree.addResistor(capFrom, tapCap, util::ohm_t(0.0));

            tapCreated[fromI]  = true;
            sourceCap = pin == source? tapCap : sourceCap;
        }

    }

    rctree.source(sourceCap);
    return sourceCap;
}


}   // namespace timingdriven_placement
}   // ophidian
