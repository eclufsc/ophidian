#include "timinggraphbuilder_test.h"
#include <catch.hpp>

#include <ophidian/design/DesignBuilder.h>
#include <ophidian/parser/LibertyParser.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/TimingGraphBuilder.h>

using namespace ophidian;

namespace
{
class TimingGraphBuilderFixture
{
public:
    design::ICCAD2015ContestDesignBuilder mBuilder;
    design::Design & mDesign;

    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    timing::TimingArcs mTimingArcs;
    timing::Library mTimingLibrary;
    std::shared_ptr<parser::DesignConstraints> mDC;

    TimingGraphBuilderFixture() :
        mBuilder("./input_files/simple/simple.lef", "./input_files/simple/simple.def", "./input_files/simple/simple.v"),
        mDesign(mBuilder.build()),
        mLiberty(ophidian::parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
        mTimingArcs(mDesign.standardCells()),
        mTimingLibrary(*mLiberty, mDesign.standardCells(), mTimingArcs, true),
        mDC(parser::SDCSimple().constraints())
    {

    }
};
} // namespace

TEST_CASE_METHOD(TimingGraphBuilderFixture, "TimingGraphBuilder: init", "[timing][TimingGraphBuilder]")
{
    timing::TimingGraphBuilder builder;

    auto graph = builder.build(mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping(), mTimingArcs, mTimingLibrary, *mDC);

    REQUIRE(graph->size(timing::TimingGraph::NodeType()) == 42);
    REQUIRE(graph->size(timing::TimingGraph::ArcType()) == 40);

    auto u1_a = mDesign.netlist().find(circuit::Pin(), "u1:a");
    auto u1_o = mDesign.netlist().find(circuit::Pin(), "u1:o");
    auto u2_a = mDesign.netlist().find(circuit::Pin(), "u2:a");

    auto riseU1_a = graph->riseNode(u1_a);
    auto riseU1_o = graph->riseNode(u1_o);
    auto fallU1_o = graph->fallNode(u1_o);
    auto riseU2_a = graph->riseNode(u2_a);

    auto timingArcU1 = mTimingArcs.find("NAND2_X1:a->NAND2_X1:o");
    auto netU1ToU2 = mDesign.netlist().find(circuit::Net(), "n1");

    REQUIRE(graph->graph().id(graph->target(graph->outArc(riseU1_a))) == graph->graph().id(fallU1_o));
    REQUIRE(graph->property(graph->outArc(riseU1_a)) == timing::TimingGraph::ArcProperty::TimingArc);
    REQUIRE(graph->entity(timing::TimingArc(), graph->outArc(riseU1_a)) == timingArcU1);

    REQUIRE(graph->graph().id(graph->target(graph->outArc(riseU1_o))) == graph->graph().id(riseU2_a));
    REQUIRE(graph->property(graph->outArc(riseU1_o)) == timing::TimingGraph::ArcProperty::Net);
    REQUIRE(graph->entity(circuit::Net(), graph->outArc(riseU1_o)) == netU1ToU2);
}
