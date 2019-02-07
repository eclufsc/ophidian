#include <catch.hpp>

#include <ophidian/placement/FenceRegions.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;

class FenceRegionsFixture {
    public:
        Netlist netlist{};

        FenceRegions fence_regions{netlist};
};