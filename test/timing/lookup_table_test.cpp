
//#include <catch.hpp>

//#include <ophidian/timing/LookupTable.h>
//#include <ophidian/util/Units.h>
//#include <ophidian/design/DesignBuilder.h>
//#include <ophidian/parser/LibertyParser.h>
//#include <ophidian/circuit/LibraryMapping.h>
//#include <ophidian/timing/TimingGraphBuilder.h>
//#include <ophidian/timing/WireModels.h>
//#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>
//#include <cmath>

//using namespace ophidian;
//using capacitance_unit_type = util::farad_t;
//using time_unit_type = util::second_t;
//using LUT = timing::LookupTable<capacitance_unit_type, time_unit_type, time_unit_type>;

//namespace
//{
//class LookupTableFixture
//{
//public:
//    design::ICCAD2015ContestDesignBuilder mBuilder;
//    design::Design & mDesign;

//    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
//    timing::TimingArcs mTimingArcs;
//    timing::Library mTimingLibrary;
//    std::shared_ptr<parser::DesignConstraints> mDC;
//    std::unique_ptr<parser::Lef> mLef;

//    timing::TimingGraph mGraph;

//    template<class Value>
//    bool diff(const Value &a, const Value &b, int exp)
//    {
//        auto x = units::math::abs(a - b);
//        auto y = Value(std::pow(10, exp-1));
//        return x < y;
//    }

//    LookupTableFixture() :
//        mBuilder("./input_files/simple/simple.lef",
//                 "./input_files/simple/simple.def",
//                "./input_files/simple/simple.v",
//                 "./input_files/simple/simple_Early.lib",
//                 "./input_files/simple/simple_Late.lib",
//                 "./input_files/simple/simple.sdc"),
//        mDesign(mBuilder.build()),
//        mLiberty(parser::LibertyParser().readFile("./input_files/simple/simple_Early.lib")),
//        mTimingArcs(mDesign.standardCells()),
//        mTimingLibrary(mDesign.standardCells(), mTimingArcs),
//        mDC(parser::SDCSimple().constraints()),
//        mGraph(mDesign.netlist())
//    {
//        mTimingLibrary.init(*mLiberty, true);
//        timing::TimingGraphBuilder().build(mDesign.netlist(),
//                                           mDesign.standardCells(),
//                                           mDesign.libraryMapping(),
//                                           mTimingArcs,
//                                           mTimingLibrary,
//                                           *mDC, mGraph);
//        mLef = std::make_unique<parser::Lef>();
//        parser::LefParser lef_parser;
//        lef_parser.readFile("input_files/simple/simple.lef", mLef);
//    }
//};
//} // namespace

//TEST_CASE_METHOD(LookupTableFixture, "lookup table/interpolation", "[timing][lut]")
//{
//    mTimingLibrary.capacitance();
//}

