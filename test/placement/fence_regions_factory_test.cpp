#include <catch.hpp>

#include <ophidian/placement/FenceRegionsFactory.h>
#include <ophidian/floorplan/FloorplanFactory.h>
#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/circuit/NetlistFactory.h>

using namespace ophidian::parser;
using namespace ophidian::floorplan;
using namespace ophidian::circuit;
using namespace ophidian::placement;
using dbu_type = FenceRegions::fence_region_geometry_type::unit_type;

class DesignFixture {
public:
    Lef lef{};
    Def def{};

    Floorplan floorplan{};
    StandardCells standard_cells{};
    Netlist netlist{};
    FenceRegions fence_regions{netlist};

    DesignFixture() {
        lef.read_file("input_files/iccad17/pci_bridge32_a_md1/tech.lef");
        lef.read_file("input_files/iccad17/pci_bridge32_a_md1/cells_modified.lef");

        def.read_file("input_files/iccad17/pci_bridge32_a_md1/placed.def");

        ophidian::floorplan::factory::make_floorplan(floorplan, def, lef);

        ophidian::circuit::factory::make_standard_cells(standard_cells, lef);

        ophidian::circuit::factory::make_netlist(netlist, def, standard_cells);
    }
};

TEST_CASE_METHOD(DesignFixture, "Create fence regions for pci_bridge32_a_md1", "[placement][fence_regions_factory]") {
    ophidian::placement::factory::make_fence_regions(fence_regions, def, floorplan, netlist);

    REQUIRE(fence_regions.size_fence_region() == 4);

    auto er0 = fence_regions.find_fence_region("er0");
    
    auto er0_area = FenceRegions::fence_region_geometry_type {{
        {{dbu_type{23200}, dbu_type{0}}, {dbu_type{379200}, dbu_type{20000}}},
        {{dbu_type{87}, dbu_type{0}}, {dbu_type{23200}, dbu_type{399870}}},
        {{dbu_type{23200}, dbu_type{380000}}, {dbu_type{379200}, dbu_type{399870}}},
        {{dbu_type{379200}, dbu_type{0}}, {dbu_type{400400}, dbu_type{399870}}}
    }};

    auto area = fence_regions.area(er0);

    auto er0_members = std::vector<FenceRegions::cell_instance_type>{fence_regions.members(er0).begin(), fence_regions.members(er0).end()};

    REQUIRE(fence_regions.name(er0) == "er0");
    REQUIRE(fence_regions.area(er0) == er0_area);
    REQUIRE(er0_members.size() == 3922);

    auto cell = netlist.find_cell_instance("h0c/g64732_u0");
    REQUIRE(fence_regions.fence_region(cell) == er0);
}