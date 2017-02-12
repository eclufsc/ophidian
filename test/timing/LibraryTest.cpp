#include "LibraryTest.h"
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
