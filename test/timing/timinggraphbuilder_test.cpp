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

    std::cout << std::endl << "Netlist's Cell: " << mDesign.netlist().size(circuit::Cell()) << std::endl;
    std::cout << "Netlist's Pin: " << mDesign.netlist().size(circuit::Pin()) << std::endl;
    std::cout << "Netlist's Net: " << mDesign.netlist().size(circuit::Net()) << std::endl;
    std::cout << "StdCell's Cell: " << mDesign.standardCells().size(standard_cell::Cell()) << std::endl;
    std::cout << "StdCell's Pin: " << mDesign.standardCells().size(standard_cell::Pin()) << std::endl;
    std::cout << "TimingArcs's Arcs: " << mTimingArcs.size() << std::endl;

    standard_cell::Pin x = mDesign.libraryMapping().pinStdCell(mDesign.netlist().find(circuit::Pin(), "u3:o"));
    standard_cell::Pin y = mDesign.standardCells().find(standard_cell::Pin(), "INV_X1:o");
    REQUIRE(x == y);

    auto graph = builder.build(mDesign.netlist(), mDesign.standardCells(), mDesign.libraryMapping(), mTimingArcs, mTimingLibrary, *mDC);
    std::cout << "Graph's Nodes: " << graph->size(timing::TimingGraph::NodeType()) << std::endl;
    std::cout << "Graph's Arcs: " << graph->size(timing::TimingGraph::ArcType()) << std::endl << std::flush;


    REQUIRE(0 == 0);
}
