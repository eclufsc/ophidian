#include <catch.hpp>

#include <ophidian/placement/FenceRegions.h>

using namespace ophidian::placement;
using namespace ophidian::circuit;
using dbu_type = FenceRegions::fence_region_geometry_type::unit_type;

class FenceRegionsFixture {
    public:
        Netlist netlist{};

        FenceRegions fence_regions{netlist};

        Netlist::cell_instance_type cell1, cell2;
        FenceRegions::fence_region_type fence_region1;

        FenceRegionsFixture() {
            cell1 = netlist.add_cell_instance("cell1");
            cell2 = netlist.add_cell_instance("cell2");

            fence_region1 = fence_regions.add_fence_region("fence1");
        }
};

TEST_CASE_METHOD(FenceRegionsFixture, "Add geometry to fence region", "[placement][fence_regions]") {
    auto fence_region_box = FenceRegions::fence_region_geometry_type::box_type{{dbu_type{0}, dbu_type{0}}, 
        {dbu_type{10}, dbu_type{10}}};

    fence_regions.add_box(fence_region1, fence_region_box);

    auto & name = fence_regions.name(fence_region1);
    auto & geometry = fence_regions.area(fence_region1);

    auto expected_geometry = FenceRegions::fence_region_geometry_type{{fence_region_box}};

    REQUIRE(name == "fence1");
    REQUIRE(geometry == expected_geometry);
}

TEST_CASE_METHOD(FenceRegionsFixture, "Add one cell to fence region", "[placement][fence_regions]") {
    fence_regions.add_member(fence_region1, cell1);

    auto members = fence_regions.members(fence_region1);

    auto cell_fence = fence_regions.fence_region(cell1);

    REQUIRE(members.size() == 1);
    REQUIRE(*members.begin() == cell1);
    REQUIRE(cell_fence == fence_region1);
    REQUIRE(!fence_regions.has_fence(cell2));
}