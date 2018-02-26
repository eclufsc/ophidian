#include "timingGraph_test.h"
#include <catch.hpp>

#include <ophidian/timing/TimingGraph.h>

using namespace ophidian::timing;

namespace
{
class TimingGraphFixture
{
public:
    ophidian::circuit::Netlist mNetlist;
    ophidian::circuit::Cell cell1, cell2;
    ophidian::circuit::Pin cell1Input, cell1Output, cell2Input, cell2Output;
    ophidian::circuit::Net net1To2;
    ophidian::timing::TimingArc cell1Arc;

    TimingGraphFixture()
    {
        cell1 = mNetlist.add(ophidian::circuit::Cell(), "cell1");
        cell1Input = mNetlist.add(ophidian::circuit::Pin(), "cell1:pinInput");
        cell1Output = mNetlist.add(ophidian::circuit::Pin(), "cell1:pinOutput");
        cell2 = mNetlist.add(ophidian::circuit::Cell(), "cell2");
        cell2Input = mNetlist.add(ophidian::circuit::Pin(), "cell2:pinInput");
        cell2Output = mNetlist.add(ophidian::circuit::Pin(), "cell2:pinOutput");
        net1To2 = mNetlist.add(ophidian::circuit::Net(), "net1To2");
        mNetlist.add(cell1, cell1Input);
        mNetlist.add(cell1, cell1Output);
        mNetlist.add(cell2, cell2Input);
        mNetlist.add(cell2, cell2Output);
        mNetlist.connect(net1To2, cell1Output);
        mNetlist.connect(net1To2, cell2Input);
    }
};
} // namespace

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: init", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    REQUIRE(mNetlist.size(ophidian::circuit::Pin()) == 4);
    REQUIRE(graph.size(TimingGraph::NodeType()) == 0);
    REQUIRE(graph.size(TimingGraph::ArcType()) == 0);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating rise nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto no1 = graph.riseNodeCreate(cell1Input);
    auto no2 = graph.riseNodeCreate(cell1Output);
    REQUIRE(graph.size(TimingGraph::NodeType()) == 2);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating same rise node twice", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.riseNodeCreate(cell1Input);
    graph.riseNodeCreate(cell1Input);
    REQUIRE(graph.size(TimingGraph::NodeType()) == 1);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating fall nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.fallNodeCreate(cell1Input);
    graph.fallNodeCreate(cell1Output);
    REQUIRE(graph.size(TimingGraph::NodeType()) == 2);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating same fall node twice", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.fallNodeCreate(cell1Input);
    graph.fallNodeCreate(cell1Input);
    REQUIRE(graph.size(TimingGraph::NodeType()) == 1);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: arc from net creating", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Output);
    auto n2 = graph.riseNodeCreate(cell2Input);
    auto arc = graph.arcCreate(n1, n2, net1To2);

    REQUIRE(graph.size(TimingGraph::ArcType()) == 1);
    REQUIRE(graph.property(arc) == TimingGraph::ArcProperty::Net);
    REQUIRE(graph.source(arc) == n1);
    REQUIRE(graph.target(arc) == n2);
    REQUIRE(graph.entity(ophidian::circuit::Net(), arc) == net1To2);
    REQUIRE_THROWS(graph.entity(TimingArc(), arc));
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: arc from timing arc creating", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto arc = graph.arcCreate(n1, n2, cell1Arc);

    REQUIRE(graph.size(TimingGraph::ArcType()) == 1);
    REQUIRE(graph.property(arc) == TimingGraph::ArcProperty::TimingArc);
    REQUIRE(graph.source(arc) == n1);
    REQUIRE(graph.target(arc) == n2);
    REQUIRE(graph.entity(TimingArc(), arc) == cell1Arc);
    REQUIRE_THROWS(graph.entity(ophidian::circuit::Net(), arc));
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: edges of a node", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    auto timingArc = graph.arcCreate(n1, n2, cell1Arc);
    auto net = graph.arcCreate(n2, n3, net1To2);

    REQUIRE(graph.size(TimingGraph::ArcType()) == 2);

    const TimingGraph::GraphType & g = graph.graph();

    REQUIRE(graph.inArc(n1) == lemon::INVALID);
    REQUIRE(g.id(graph.outArc(n1)) == g.id(timingArc));

    REQUIRE(g.id(graph.inArc(n2)) == g.id(timingArc));
    REQUIRE(g.id(graph.outArc(n2)) == g.id(net));

    REQUIRE(g.id(graph.inArc(n3)) == g.id(net));
    REQUIRE(graph.outArc(n3) == lemon::INVALID);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: removing nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);
    std::vector<TimingGraph::NodeType> remove;

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    graph.arcCreate(n1, n2, cell1Arc);
    graph.arcCreate(n2, n3, net1To2);

    remove.push_back(n1);
    remove.push_back(n2);
    remove.push_back(n3);

    REQUIRE(graph.size(TimingGraph::NodeType()) == 3);
    REQUIRE(graph.size(TimingGraph::ArcType()) == 2);

    graph.destroy(remove.begin(), remove.end());

    REQUIRE(graph.size(TimingGraph::NodeType()) == 0);
    REQUIRE(graph.size(TimingGraph::ArcType()) == 0);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: removing edges", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);
    std::vector<TimingGraph::ArcType> remove;

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    remove.push_back(graph.arcCreate(n1, n2, cell1Arc));
    remove.push_back(graph.arcCreate(n2, n3, net1To2));

    REQUIRE(graph.size(TimingGraph::NodeType()) == 3);
    REQUIRE(graph.size(TimingGraph::ArcType()) == 2);

    graph.destroy(remove.begin(), remove.end());

    REQUIRE(graph.size(TimingGraph::NodeType()) == 3);
    REQUIRE(graph.size(TimingGraph::ArcType()) == 0);
}

