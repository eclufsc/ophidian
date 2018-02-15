#include "timing_library_test.h"
#include <catch.hpp>

#include <ophidian/timing/Library.h>

namespace
{
class LibraryFixture
{
public:
    std::string inverterName;
    std::string invInputName, invOutputName;
    std::shared_ptr<ophidian::parser::Liberty> mLiberty;
    ophidian::standard_cell::StandardCells mStdCells;
    ophidian::timing::TimingArcs mArcs;

    LibraryFixture() :
        inverterName("INV_X1"),
        invInputName("a"),
        invOutputName("o"),
        mLiberty(ophidian::parser::LibertyParser().readFile("./input_files/sample2_Late.lib"))
    {
        auto inv = mStdCells.add(ophidian::standard_cell::Cell(), inverterName);
        auto in = mStdCells.add(ophidian::standard_cell::Pin(), inverterName+":"+invInputName, ophidian::standard_cell::PinDirection::INPUT);
        auto out = mStdCells.add(ophidian::standard_cell::Pin(), inverterName+":"+invOutputName, ophidian::standard_cell::PinDirection::OUTPUT);
        mStdCells.add(inv, in);
        mStdCells.add(inv, out);
    }
};
} // namespace

TEST_CASE_METHOD(LibraryFixture, "Library: init", "[timing][library]")
{
    REQUIRE(mArcs.size() == 0);
    REQUIRE(mStdCells.size(ophidian::standard_cell::Cell()) == 1);
    REQUIRE(mStdCells.size(ophidian::standard_cell::Pin()) == 2);

    ophidian::timing::Library lib(*mLiberty.get(), mStdCells, mArcs);

    REQUIRE(mArcs.size() == 1);
    REQUIRE(mStdCells.size(ophidian::standard_cell::Cell()) == 1);
    REQUIRE(mStdCells.size(ophidian::standard_cell::Pin()) == 2);
}
