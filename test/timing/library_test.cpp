#include "library_test.h"
#include <catch.hpp>

#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/timing/Library.h>
#include <ophidian/util/Units.h>

using namespace ophidian::standard_cell;
using namespace ophidian::timing;
using namespace ophidian::util;

namespace {

class TimingLibraryFixture
{
public:
    TimingLibraryFixture() :
        lib_(cells_)
    {
        INV_X1a_ = cells_.add(Pin{}, "INV_X1:a", PinDirection::INPUT);
        INV_X1o_ = cells_.add(Pin{}, "INV_X1:o", PinDirection::OUTPUT);
    }
    virtual ~TimingLibraryFixture()
    {

    }
protected:
    StandardCells cells_;
    Library lib_;
    Pin INV_X1a_;
    Pin INV_X1o_;
};

class TimingLibraryWithPinTimingFixture: public TimingLibraryFixture
{
public:
    TimingLibraryWithPinTimingFixture() :
        timing_(lib_.add(Library::Timing{}, INV_X1o_))
    {
    }
    virtual ~TimingLibraryWithPinTimingFixture()
    {

    }
protected:
    Library::Timing timing_;
};


}

TEST_CASE("Timing/Library Ctor", "[timing][Library]")
{
    StandardCells cells;
    REQUIRE_NOTHROW(Library(cells));
}

TEST_CASE_METHOD(TimingLibraryFixture, "Timing/Library pin capacitance", "[timing][Library]")
{
    lib_.capacitance(INV_X1a_, femtofarad_t{4.2});
    REQUIRE( lib_.capacitance(INV_X1a_) == femtofarad_t{4.2} );
    lib_.capacitance(INV_X1a_, picofarad_t{0.0043});
    REQUIRE( lib_.capacitance(INV_X1a_) == picofarad_t{0.0043} );
}

TEST_CASE_METHOD(TimingLibraryFixture, "Timing/Pin timing is empty", "[timing][Library]")
{
    REQUIRE(lib_.timing(INV_X1a_).empty());
}

TEST_CASE_METHOD(TimingLibraryWithPinTimingFixture, "Timing/Add pin timing", "[timing][Library]")
{
    REQUIRE(!lib_.timing(INV_X1o_).empty());
    REQUIRE(lib_.timing(INV_X1o_).size() == 1);
}

TEST_CASE_METHOD(TimingLibraryWithPinTimingFixture, "Timing/Pin timing related pin", "[timing][Library]")
{
    REQUIRE(lib_.relatedPin(timing_) == Pin{});
    lib_.relatedPin(timing_, INV_X1a_);
    REQUIRE(lib_.relatedPin(timing_) == INV_X1a_);
}

TEST_CASE_METHOD(TimingLibraryWithPinTimingFixture, "Timing/Pin timing unateness", "[timing][Library]")
{
    REQUIRE(lib_.timingSense(timing_) == Unateness::NEGATIVE_UNATE);
    lib_.timingSense(timing_, Unateness::NON_UNATE);
    REQUIRE(lib_.timingSense(timing_) == Unateness::NON_UNATE);
}

TEST_CASE_METHOD(TimingLibraryWithPinTimingFixture, "Timing/Pin timing lookup tables", "[timing][Library]")
{
    REQUIRE( lib_.lookupTables(timing_).empty() );
    Library::LookupTable lut;
    REQUIRE( lib_.lookupTables(timing_).count("cell_rise") == 0 );
    lib_.lookupTables(timing_).insert(std::make_pair("cell_rise", lut));
    REQUIRE( lib_.lookupTables(timing_).count("cell_rise") == 1 );
}

TEST_CASE_METHOD(TimingLibraryWithPinTimingFixture, "Timing/Pin timing type", "[timing][Library]")
{
    INFO( "Default timing type = Combinational" );
    REQUIRE( lib_.timingType(timing_) == TimingType::COMBINATIONAL );
    lib_.timingType(timing_, TimingType::HOLD_RISING);
    auto dummyTiming = lib_.add(Library::Timing{}, INV_X1o_);
    REQUIRE( lib_.timing(INV_X1o_).size() == 2 );
    REQUIRE( lib_.timingType(dummyTiming) == TimingType::COMBINATIONAL );
    REQUIRE( lib_.timingType(timing_) == TimingType::HOLD_RISING );

}

TEST_CASE("Timing/Test library", "[timing][Library]")
{
    using ophidian::util::femtofarad_t;
    StandardCells stdCells;

    auto DFF_X2 = stdCells.add(Cell{}, "DFF_X2");
    auto DFF_X2q = stdCells.add(Pin{}, "q", PinDirection::OUTPUT);
    auto DFF_X2ck = stdCells.add(Pin{}, "ck", PinDirection::INPUT);
    auto DFF_X2d = stdCells.add(Pin{}, "d", PinDirection::INPUT);
    stdCells.add(DFF_X2, DFF_X2q);
    stdCells.add(DFF_X2, DFF_X2ck);
    stdCells.add(DFF_X2, DFF_X2d);

    Library lib(stdCells);
    lib.capacitance(DFF_X2d, femtofarad_t{3.49});
    lib.capacitance(DFF_X2ck, femtofarad_t{1.5});
    lib.capacitance(DFF_X2q, std::numeric_limits<femtofarad_t>::max());

    auto timingArcCkToQ = lib.add(Library::Timing{}, DFF_X2q);
    auto timingArcSetup = lib.add(Library::Timing{}, DFF_X2d);
    auto timingArcHold = lib.add(Library::Timing{}, DFF_X2d);

    lib.relatedPin(timingArcCkToQ, DFF_X2ck);
    lib.timingSense(timingArcCkToQ, Unateness::NON_UNATE);
    lib.timingType(timingArcCkToQ, TimingType::COMBINATIONAL);
    lib.lookupTables(timingArcCkToQ).insert(std::make_pair("cell_rise", Library::LookupTable{}));
    lib.lookupTables(timingArcCkToQ).insert(std::make_pair("cell_fall", Library::LookupTable{}));
    lib.lookupTables(timingArcCkToQ).insert(std::make_pair("rise_transition", Library::LookupTable{}));
    lib.lookupTables(timingArcCkToQ).insert(std::make_pair("fall_transition", Library::LookupTable{}));

    lib.relatedPin(timingArcSetup, DFF_X2ck);
    lib.timingSense(timingArcSetup, Unateness::POSITIVE_UNATE); /* TODO: does it make sense? */
    lib.timingType(timingArcSetup, TimingType::SETUP_RISING);
    lib.lookupTables(timingArcSetup).insert(std::make_pair("rise_constraint", Library::LookupTable{}));
    lib.lookupTables(timingArcSetup).insert(std::make_pair("fall_constraint", Library::LookupTable{}));

    lib.relatedPin(timingArcHold, DFF_X2ck);
    lib.timingSense(timingArcHold, Unateness::POSITIVE_UNATE); /* TODO: does it make sense? */
    lib.timingType(timingArcHold, TimingType::HOLD_RISING);
    lib.lookupTables(timingArcHold).insert(std::make_pair("rise_constraint", Library::LookupTable{}));
    lib.lookupTables(timingArcHold).insert(std::make_pair("fall_constraint", Library::LookupTable{}));

    REQUIRE( lib.capacitance(DFF_X2d) == femtofarad_t{3.49} );
    REQUIRE( lib.capacitance(DFF_X2ck) == femtofarad_t{1.5} );

    REQUIRE( lib.timing(DFF_X2q).size() == 1 );

    REQUIRE( lib.timingType(timingArcCkToQ) == TimingType::COMBINATIONAL );
    REQUIRE( lib.relatedPin(timingArcCkToQ) == DFF_X2ck );
    REQUIRE( lib.timingSense(timingArcCkToQ) == Unateness::NON_UNATE );
    REQUIRE( lib.lookupTables(timingArcCkToQ).size() == 4 );
    REQUIRE( lib.lookupTables(timingArcCkToQ).count("cell_rise") == 1);
    REQUIRE( lib.lookupTables(timingArcCkToQ).count("cell_fall") == 1);
    REQUIRE( lib.lookupTables(timingArcCkToQ).count("rise_transition") == 1);
    REQUIRE( lib.lookupTables(timingArcCkToQ).count("fall_transition") == 1);

    REQUIRE( lib.timing(DFF_X2d).size() == 2 );

    REQUIRE( lib.timingType(timingArcSetup) == TimingType::SETUP_RISING );
    REQUIRE( lib.relatedPin(timingArcSetup) == DFF_X2ck );
    REQUIRE( lib.timingSense(timingArcSetup) == Unateness::POSITIVE_UNATE );
    REQUIRE( lib.lookupTables(timingArcSetup).size() == 2 );
    REQUIRE( lib.lookupTables(timingArcSetup).count("rise_constraint") == 1 );
    REQUIRE( lib.lookupTables(timingArcSetup).count("fall_constraint") == 1 );

    REQUIRE( lib.timingType(timingArcHold) == TimingType::HOLD_RISING );
    REQUIRE( lib.relatedPin(timingArcHold) == DFF_X2ck );
    REQUIRE( lib.timingSense(timingArcHold) == Unateness::POSITIVE_UNATE );
    REQUIRE( lib.lookupTables(timingArcHold).size() == 2 );
    REQUIRE( lib.lookupTables(timingArcHold).count("rise_constraint") == 1 );
    REQUIRE( lib.lookupTables(timingArcHold).count("fall_constraint") == 1 );

}

