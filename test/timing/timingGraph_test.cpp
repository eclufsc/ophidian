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
    ophidian::timing::Arc cell1Arc;

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
    REQUIRE(graph.nodesAmount() == 0);
    REQUIRE(graph.edgesAmount() == 0);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating rise nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto no1 = graph.riseNodeCreate(cell1Input);
    auto no2 = graph.riseNodeCreate(cell1Output);
    REQUIRE(graph.nodesAmount() == 2);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating same rise node twice", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.riseNodeCreate(cell1Input);
    graph.riseNodeCreate(cell1Input);
    REQUIRE(graph.nodesAmount() == 1);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating fall nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.fallNodeCreate(cell1Input);
    graph.fallNodeCreate(cell1Output);
    REQUIRE(graph.nodesAmount() == 2);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: creating same fall node twice", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    graph.fallNodeCreate(cell1Input);
    graph.fallNodeCreate(cell1Input);
    REQUIRE(graph.nodesAmount() == 1);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: edge from net creating", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Output);
    auto n2 = graph.riseNodeCreate(cell2Input);
    auto edge = graph.edgeCreate(n1, n2, *static_cast<Edge*>(&net1To2), edge_types::Net);

    REQUIRE(graph.edgesAmount() == 1);
    REQUIRE(graph.type(edge) == edge_types::Net);
    REQUIRE(graph.source(edge) == n1);
    REQUIRE(graph.target(edge) == n2);
    REQUIRE(graph.entity(ophidian::circuit::Net(), edge) == net1To2);
    REQUIRE_THROWS(graph.entity(Arc(), edge));
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: edge from timing arc creating", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto edge = graph.edgeCreate(n1, n2, *static_cast<Edge*>(&cell1Arc), edge_types::TimingArc);

    REQUIRE(graph.edgesAmount() == 1);
    REQUIRE(graph.type(edge) == edge_types::TimingArc);
    REQUIRE(graph.source(edge) == n1);
    REQUIRE(graph.target(edge) == n2);
    REQUIRE(graph.entity(Arc(), edge) == cell1Arc);
    REQUIRE_THROWS(graph.entity(ophidian::circuit::Net(), edge));
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: edges of a node", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    auto timingArc = graph.edgeCreate(n1, n2, *static_cast<Edge*>(&cell1Arc), edge_types::TimingArc);
    auto net = graph.edgeCreate(n2, n3, *static_cast<Edge*>(&net1To2), edge_types::Net);

    REQUIRE(graph.edgesAmount() == 2);

    const TimingGraph::graph_t & g = graph.graph();

    REQUIRE(graph.inEdge(n1) == lemon::INVALID);
    REQUIRE(g.id(graph.outEdge(n1)) == g.id(timingArc));

    REQUIRE(g.id(graph.inEdge(n2)) == g.id(timingArc));
    REQUIRE(g.id(graph.outEdge(n2)) == g.id(net));

    REQUIRE(g.id(graph.inEdge(n3)) == g.id(net));
    REQUIRE(graph.outEdge(n3) == lemon::INVALID);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: removing nodes", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);
    std::vector<TimingGraph::node_t> remove;

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    graph.edgeCreate(n1, n2, *static_cast<Edge*>(&cell1Arc), edge_types::TimingArc);
    graph.edgeCreate(n2, n3, *static_cast<Edge*>(&net1To2), edge_types::Net);

    remove.push_back(n1);
    remove.push_back(n2);
    remove.push_back(n3);

    REQUIRE(graph.nodesAmount() == 3);
    REQUIRE(graph.edgesAmount() == 2);

    graph.destroy(remove.begin(), remove.end());

    REQUIRE(graph.nodesAmount() == 0);
    REQUIRE(graph.edgesAmount() == 0);
}

TEST_CASE_METHOD(TimingGraphFixture, "TimingGraph: removing edges", "[timing][TimingGraph]")
{
    TimingGraph graph(mNetlist);
    std::vector<TimingGraph::edge_t> remove;

    auto n1 = graph.riseNodeCreate(cell1Input);
    auto n2 = graph.riseNodeCreate(cell1Output);
    auto n3 = graph.riseNodeCreate(cell2Input);
    remove.push_back(graph.edgeCreate(n1, n2, *static_cast<Edge*>(&cell1Arc), edge_types::TimingArc));
    remove.push_back(graph.edgeCreate(n2, n3, *static_cast<Edge*>(&net1To2), edge_types::Net));

    REQUIRE(graph.nodesAmount() == 3);
    REQUIRE(graph.edgesAmount() == 2);

    graph.destroy(remove.begin(), remove.end());

    REQUIRE(graph.nodesAmount() == 3);
    REQUIRE(graph.edgesAmount() == 0);
}

