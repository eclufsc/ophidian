#include "TimingGraphTest.h"
#include <catch.hpp>

#include <ophidian/timing/Graph.h>

using namespace ophidian::timing;

struct Element : public ophidian::entity_system::EntityBase {
    using ophidian::entity_system::EntityBase::EntityBase;
    void setId(uint32_t id) {
        id_ = id;
    }
};

static const Element kInvalid{};

TEST_CASE("timing graph: create an empty graph", "[timing]") {
    Graph<Element> g(0, kInvalid);
    REQUIRE( g.empty() );
    REQUIRE( g.numNodes() == 0 );
    REQUIRE( g.numArcs() == 0 );
}

TEST_CASE("timing graph: size", "[timing]") {
    Graph<Element> g(123, kInvalid);
    REQUIRE( g.numNodes() == 123 );
    REQUIRE( !g.empty() );
}

TEST_CASE("timing graph: get node by id", "[timing]") {
    Graph<Element> g(1024, kInvalid);
    auto u1 = g.node(0);
    auto u2 = g.node(1);
    auto u1024 = g.node(1023);
    REQUIRE( g.id(u1) == 0 );
    REQUIRE( g.id(u2) == 1 );
    REQUIRE( g.id(u1024) == 1023 );
}

TEST_CASE("timing graph: can't get invalid  node", "[timing]") {
    Graph<Element> g(1024, kInvalid);
    REQUIRE_THROWS(g.node(1024));
}

TEST_CASE("timing graph: unlinked nodes", "[timing]") {
    Graph<Element> g(4, kInvalid);
    REQUIRE( g.nodeEntity(g.node(0)) == kInvalid );
    REQUIRE( g.nodeEntity(g.node(1)) == kInvalid );
    REQUIRE( g.nodeEntity(g.node(2)) == kInvalid );
    REQUIRE( g.nodeEntity(g.node(3)) == kInvalid );
    REQUIRE( g.nodeEdge(g.node(0)) == Edges::kUnlinked );
    REQUIRE( g.nodeEdge(g.node(1)) == Edges::kUnlinked );
    REQUIRE( g.nodeEdge(g.node(2)) == Edges::kUnlinked );
    REQUIRE( g.nodeEdge(g.node(3)) == Edges::kUnlinked );
}

TEST_CASE("timing graph: link rise node", "[timing]") {
    Graph<Element> g(4, kInvalid);
    Element thePin;
    thePin.setId(0);
    g.linkRiseNode(g.node(0), thePin);
    REQUIRE( g.nodeEntity(g.node(0)) == thePin );
    REQUIRE( g.nodeEntity(g.node(1)) == kInvalid );
    REQUIRE( g.nodeEntity(g.node(2)) == kInvalid );
    REQUIRE( g.nodeEntity(g.node(3)) == kInvalid );
    REQUIRE( g.nodeEdge(g.node(0)) == Edges::kRise );
}

TEST_CASE("timing graph: connect nodes", "[timing]") {
    Graph<Element> g(2, kInvalid);
    auto n1 = g.node(0);
    auto n2 = g.node(1);
    auto edge = g.connect(n1, n2);
    REQUIRE( g.numArcs() == 1 );
    REQUIRE( g.source(edge) == n1 );
    REQUIRE( g.target(edge) == n2 );
}

TEST_CASE("timing graph: make nodes property", "[timing]") {
    Graph<Element> g(2, kInvalid);
    auto prop = g.makeProperty<double>(Graph<Element>::Node());
    (*prop)[g.node(0)] = 1.0;
    (*prop)[g.node(1)] = 2.0;
    REQUIRE( Approx((*prop)[g.node(0)]) == 1.0 );
    REQUIRE( Approx((*prop)[g.node(1)]) == 2.0 );
}

#include <ophidian/circuit/Netlist.h>
TEST_CASE("timing graph: rise and fall maps", "[timing]") {
    using namespace ophidian::circuit;
    Netlist ckt;
    static Pin kPin;
    auto a = ckt.add(kPin);
    Graph<Pin> g(2*ckt.size(kPin), kPin);
    auto riseNode_ = ckt.makeProperty<Graph<Pin>::Node>(kPin);
    g.setRiseMap(riseNode_);
    auto fallNode_ = ckt.makeProperty<Graph<Pin>::Node>(kPin);
    g.setRiseMap(fallNode_);
    g.linkRiseNode(g.node(0), a);
    g.linkFallNode(g.node(1), a);
    auto & riseNode = *riseNode_;
    REQUIRE( riseNode[a] == g.node(0) );
    auto & fallNode = *fallNode_;
    REQUIRE( fallNode[a] == g.node(0) );
}

#include <ophidian/circuit/Netlist.h>
TEST_CASE("timing graph: negative unateness arc example", "[timing]") {
    using namespace ophidian::circuit;
    static Pin kPin;

    // instantiate an netlist with two pins
    Netlist ckt;
    auto a = ckt.add(kPin);
    auto o = ckt.add(kPin);

    // instantiate the timing graph with 4 nodes
    Graph<Pin> g(2*ckt.size(kPin), kPin);

    // set rise and fall maps
    auto riseNode_ = ckt.makeProperty<Graph<Pin>::Node>(kPin);
    auto fallNode_ = ckt.makeProperty<Graph<Pin>::Node>(kPin);
    g.setRiseMap(riseNode_);
    g.setFallMap(fallNode_);

    // link rise and fall nodes
    g.linkRiseNode(g.node(0), a);
    g.linkFallNode(g.node(1), o);
    g.linkFallNode(g.node(2), a);
    g.linkRiseNode(g.node(3), o);

    // create negative-unate arcs
    auto & riseNode = *riseNode_;
    auto & fallNode = *fallNode_;
    auto arcFall = g.connect(riseNode[a], fallNode[o]);
    auto arcRise = g.connect(fallNode[a], riseNode[o]);

    REQUIRE( g.nodeEdge(g.source(arcFall)) == Edges::kRise );
    REQUIRE( g.nodeEdge(g.source(arcRise)) == Edges::kFall );
    REQUIRE( g.nodeEdge(g.target(arcFall)) == Edges::kFall );
    REQUIRE( g.nodeEdge(g.target(arcRise)) == Edges::kRise );
    REQUIRE( g.nodeEdge(riseNode[a]) == Edges::kRise );
    REQUIRE( g.nodeEdge(fallNode[o]) == Edges::kFall );
    REQUIRE( g.nodeEdge(fallNode[a]) == Edges::kFall );
    REQUIRE( g.nodeEdge(riseNode[o]) == Edges::kRise );
}

#include <algorithm>
TEST_CASE("timing graph: node iterator", "[timing]") {
    using namespace ophidian::circuit;
    static Pin kPin;

    Graph<Pin> g(3, kPin);
    auto count = std::count_if(g.begin(Graph<Pin>::Node{}), g.end(Graph<Pin>::Node{}), [&g](const Graph<Pin>::Node& node)->bool{
        return g.id(node) == 4;
    });
    REQUIRE( count == 0 );

    count = std::count_if(g.begin(Graph<Pin>::Node{}), g.end(Graph<Pin>::Node{}), [&g](const Graph<Pin>::Node& node)->bool{
        return g.id(node) < 3;
    });
    REQUIRE( count == 3 );
}

TEST_CASE("timing graph: node arcs iterator", "[timing]") {
    using namespace ophidian::circuit;
    static Pin kPin;

    Graph<Pin> g(4, kPin);
    auto arc1 = g.connect(g.node(0), g.node(1));
    auto arc2 = g.connect(g.node(0), g.node(2));
    auto arc3 = g.connect(g.node(0), g.node(3));

    REQUIRE( std::distance(g.outArcsBegin(g.node(0)), g.outArcsEnd(g.node(0))) == 3 );
    REQUIRE( std::count(g.inArcsBegin(g.node(1)), g.inArcsEnd(g.node(1)), arc1) == 1 );
    REQUIRE( std::count(g.inArcsBegin(g.node(2)), g.inArcsEnd(g.node(2)), arc2) == 1 );
    REQUIRE( std::count(g.inArcsBegin(g.node(3)), g.inArcsEnd(g.node(3)), arc3) == 1 );

}

