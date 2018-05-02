#include <ophidian/timing/GenericSTA.h>

namespace ophidian
{
namespace timing
{

    TimingData::TimingData(const Library& lib, const TimingGraph& graph) :
        mLibrary(lib),
        mNodesTiming(graph),
        mArcsTiming(graph)
    {
    }


    GraphAndTopology::GraphAndTopology(const TimingGraph & graph,
                                       circuit::Netlist & netlist,
                                       standard_cell::StandardCells & stdCells,
                                       circuit::LibraryMapping & libraryMapping) :
        mGraph(graph),
        mNetlist(netlist),
        mSorted(mGraph.size(TimingGraph::NodeType())),
        mSortedDrivers(mGraph.size(TimingGraph::NodeType()))
    {
        using GraphType = TimingGraph::GraphType;

        GraphType::NodeMap<int> order(mGraph.graph());
        lemon::topologicalSort(mGraph.graph(), order);

        GraphType::NodeMap<int> level(mGraph.graph());

        for(GraphType::NodeIt it(mGraph.graph()); it != lemon::INVALID; ++it)
        {
            level[it] = std::numeric_limits<int>::max();
            mSorted[ order[it] ] = it;
            mSortedDrivers[ order[it] ] = it;
            if(lemon::countInArcs(mGraph.graph(), it) == 0)
                level[it] = 0;
        }

        int numLevels = 0;
        for(auto node : mSorted)
        {
            if(lemon::countInArcs(mGraph.graph(), node) > 0)
            {
                int maxLevel = std::numeric_limits<int>::min();
                for(GraphType::InArcIt arc(mGraph.graph(), node); arc != lemon::INVALID; ++arc)
                    maxLevel = std::max(maxLevel, level[mGraph.source(arc)]);
                level[node] = maxLevel + 1;
                numLevels = std::max(numLevels, maxLevel + 1);
            }
        }

        mLevels.resize(numLevels+1);

        for(auto node : mSorted)
            if(stdCells.direction(libraryMapping.pinStdCell(mGraph.entity(node))) == standard_cell::PinDirection::OUTPUT)
                mLevels[level[node]].push_back(node);

        auto beg = std::remove_if
        (
            mLevels.begin(), mLevels.end(),
            [this](std::vector<lemon::ListDigraph::Node> & vec)->bool
            {
                return vec.empty();
            }
        );

        mLevels.erase(beg, mLevels.end());


        auto begin = std::remove_if
        (
            mSortedDrivers.begin(),
            mSortedDrivers.end(),
            [this, &stdCells, &libraryMapping](GraphType::Node node)->bool
            {
                return stdCells.direction(libraryMapping.pinStdCell(mGraph.entity(node))) == standard_cell::PinDirection::OUTPUT;
            }
        );

        mSortedDrivers.erase(begin, mSortedDrivers.end());
    }

}   // namespace timing
}   // namespace ophidian
