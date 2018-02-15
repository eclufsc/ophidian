#include "timing_library_test.h"
#include <catch.hpp>

#include <ophidian/timing/Library.h>

namespace
{
class LibraryFixture
{
public:
    ophidian::timing::TimingArcs mArcs;
    ophidian::standard_cell::StandardCells mStdCells;
    std::shared_ptr<ophidian::parser::Liberty> mLiberty;

    LibraryFixture() :
        mLiberty(ophidian::parser::LibertyParser().readFile("./input_files/sample2_Late.lib"))
    {
        auto inv = mStdCells.add(ophidian::standard_cell::Cell(), "INV_X1");
        auto invIn = mStdCells.add(ophidian::standard_cell::Pin(), "INV_X1:a", ophidian::standard_cell::PinDirection::INPUT);
        auto invOut = mStdCells.add(ophidian::standard_cell::Pin(), "INV_X1:o", ophidian::standard_cell::PinDirection::OUTPUT);
        mStdCells.add(inv, invIn);
        mStdCells.add(inv, invOut);

        auto ff = mStdCells.add(ophidian::standard_cell::Cell(), "DFF_X80");
        auto ffCk = mStdCells.add(ophidian::standard_cell::Pin(), "DFF_X80:ck", ophidian::standard_cell::PinDirection::INOUT);
        auto ffIn = mStdCells.add(ophidian::standard_cell::Pin(), "DFF_X80:d", ophidian::standard_cell::PinDirection::INPUT);
        auto ffOut = mStdCells.add(ophidian::standard_cell::Pin(), "DFF_X80:q", ophidian::standard_cell::PinDirection::OUTPUT);
        mStdCells.add(ff, ffCk);
        mStdCells.add(ff, ffIn);
        mStdCells.add(ff, ffOut);
    }
};
} // namespace

TEST_CASE_METHOD(LibraryFixture, "Library: init", "[timing][library]")
{
    REQUIRE(mArcs.size() == 0);

    ophidian::timing::Library lib(*mLiberty.get(), mStdCells, mArcs);

    REQUIRE(mArcs.size() == 4);

    int i = 0;
    for (auto arcIt = mArcs.begin(); arcIt != mArcs.end(); arcIt++, i++)
    {
        auto arc = *arcIt;
        switch (i) {
        case 0:
            REQUIRE(lib.unateness(arc) == ophidian::timing::unateness_t::NEGATIVE_UNATE);
            REQUIRE(lib.type(arc) == ophidian::timing::timing_type_t::COMBINATIONAL);
            REQUIRE(lib.computeRiseDelay(arc, 1.5, 20.0) == 28.116);
            REQUIRE(lib.computeFallDelay(arc, 0.75, 325.0) == 71.244375);
            REQUIRE(lib.computeRiseSlews(arc, 18.5, 18.5) == 153.75);
            REQUIRE(lib.computeFallSlews(arc, 8.0, 300.0) == 106.536);
            break;
        case 1:
            REQUIRE(lib.unateness(arc) == ophidian::timing::unateness_t::NON_UNATE);
            REQUIRE(lib.type(arc) == ophidian::timing::timing_type_t::RISING_EDGE);
            REQUIRE(lib.computeRiseDelay(arc, 128.0, 30.0) == 25.2);
            REQUIRE(lib.computeFallDelay(arc, 2048.0, 300.0) == 115.2);
            REQUIRE(lib.computeRiseSlews(arc, 512.0, 200.0) == 43.2);
            REQUIRE(lib.computeFallSlews(arc, 32.0, 32.0) == 20.7);
            break;
        case 2:
            REQUIRE(lib.type(arc) == ophidian::timing::timing_type_t::SETUP_RISING);
            REQUIRE(lib.computeRiseSlews(arc, 1, 1) == 1.5);
            REQUIRE(lib.computeFallSlews(arc, 1, 1) == 2.5);
            break;
        case 3:
            REQUIRE(lib.type(arc) == ophidian::timing::timing_type_t::HOLD_RISING);
            REQUIRE(lib.computeRiseSlews(arc, 1, 1) == 3.5);
            REQUIRE(lib.computeFallSlews(arc, 1, 1) == 4.5);
            break;
        default:
            break;
        }
    }
}
