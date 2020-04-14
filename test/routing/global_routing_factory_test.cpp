#include <catch.hpp>
#include <vector>

#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/circuit/NetlistFactory.h>
#include <ophidian/placement/PlacementFactory.h>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/GlobalRoutingFactory.h>
#include <ophidian/routing/LibraryFactory.h>
#include <ophidian/util/Units.h>
#include <ophidian/geometry/Models.h>

#include <algorithm>
using namespace ophidian::parser;

using namespace ophidian::util;
using namespace ophidian::geometry;
using unit_type = database_unit_t;
using point_type = Point<unit_type>;
using box_type = Box<unit_type>;

bool boxComparator(const box_type& a, const box_type& b)
{
    return a.min_corner().x() == b.min_corner().x() && a.max_corner().x() == b.max_corner().x() &&
            a.min_corner().y() == b.min_corner().y() && a.max_corner().y() == b.max_corner().y();
}

bool pairComparator(const std::pair<box_type, std::string>& a, const std::pair<box_type, std::string>& b)
{
    return a.second == b.second && boxComparator(a.first,b.first);
}

TEST_CASE("Global Routing Factory Test", "[routing][globalRouting][factory]")
{
    Def sample_def = ophidian::parser::Def{"input_files/ispd18/ispd18_sample/ispd18_sample.input.def"};
    Lef sample_lef = ophidian::parser::Lef{"input_files/ispd18/ispd18_sample/ispd18_sample.input.lef"};
    Guide sample_guide = ophidian::parser::Guide{"input_files/ispd18/ispd18_sample/ispd18_sample.input.guide"};

    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_ispd2018(design, sample_def, sample_lef, sample_guide);

    auto& globalRouting = design.global_routing();
    CHECK(globalRouting.size_segment() == 52);

    auto net = design.netlist().find_net("net1235");
    auto segments = globalRouting.segments(net);
    CHECK(segments.size() == 4);

    std::vector<std::pair<box_type, std::string>> global_routing_segments;
    for(auto part : segments){
        auto box = globalRouting.box(part);
        auto layer_instance = globalRouting.layer_start(part);
        auto layer_name = design.routing_library().name(layer_instance);
        global_routing_segments.push_back(std::make_pair(box, layer_name));
    }

    std::vector<std::pair<box_type, std::string>> expected_segments;
    expected_segments.push_back(std::make_pair(box_type{point_type{unit_type{95600}, unit_type{71820}}, point_type{unit_type{104400}, unit_type{77520}}}, "Metal1"));
    expected_segments.push_back(std::make_pair(box_type{point_type{unit_type{95600}, unit_type{83220}}, point_type{unit_type{104400}, unit_type{91200}}}, "Metal1"));
    expected_segments.push_back(std::make_pair(box_type{point_type{unit_type{95600}, unit_type{71820}}, point_type{unit_type{104400}, unit_type{91200}}}, "Metal2"));
    expected_segments.push_back(std::make_pair(box_type{point_type{unit_type{95600}, unit_type{77520}}, point_type{unit_type{104400}, unit_type{83220}}}, "Metal3"));

    CHECK(std::is_permutation(expected_segments.begin(), expected_segments.end(), global_routing_segments.begin(), pairComparator));
}

TEST_CASE("Make a global routing from an iccad2020 file.", "[routing][globalRouting][factory]")
{
    ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_iccad2020(design, sample);

    auto & netlist = design.netlist();
    auto & global_routing = design.global_routing();
    auto graph_ptr = global_routing.gcell_graph();
    auto & library = design.routing_library();

    CHECK(global_routing.size_segment() == 42);
    SECTION("Net N3 segments")
    {
        auto net_N3 = netlist.find_net("N3");
        auto segments = global_routing.segments(net_N3);
        std::vector<box_type> box_segments;
        std::vector<box_type> expected_box_segments{{point_type{unit_type{10}, unit_type{10}}, point_type{unit_type{20}, unit_type{20}}},
                                                    {point_type{unit_type{10}, unit_type{10}}, point_type{unit_type{40}, unit_type{20}}},
                                                    {point_type{unit_type{30}, unit_type{10}}, point_type{unit_type{40}, unit_type{20}}},
                                                    {point_type{unit_type{10}, unit_type{10}}, point_type{unit_type{20}, unit_type{50}}}};
        box_segments.reserve(segments.size());
        for(auto segment : segments)
            box_segments.push_back(global_routing.box(segment));
        REQUIRE(segments.size() == 4);
        REQUIRE(std::is_permutation(box_segments.begin(), box_segments.end(), expected_box_segments.begin(), boxComparator));
    }

    SECTION("Horizontal segment properties")
    {
        auto net_N3 = netlist.find_net("N3");
        auto segments = global_routing.segments(net_N3);
        auto box = box_type{point_type{unit_type{10}, unit_type{10}}, point_type{unit_type{40}, unit_type{20}}};
        auto segment_it = std::find_if(segments.begin(),
                                       segments.end(),
                                       [&](auto & seg){return boxComparator(global_routing.box(seg), box);});

        auto start_layer = global_routing.layer_start(*segment_it);
        auto end_layer = global_routing.layer_end(*segment_it);
        REQUIRE(start_layer == end_layer);
        REQUIRE(library.layerIndex(start_layer) == 3);
        auto start_gcell = global_routing.gcell_start(*segment_it);
        auto end_gcell = global_routing.gcell_end(*segment_it);

        auto s_graph_node = graph_ptr->graph_node(start_gcell);
        auto s_node_position = graph_ptr->position(s_graph_node);

        auto e_graph_node = graph_ptr->graph_node(end_gcell);
        auto e_node_position = graph_ptr->position(e_graph_node);

        std::vector<std::tuple<int,int,int>> expected_nodes={{1,1,2}, {3,1,2}};
        std::vector<std::tuple<int,int,int>> nodes;
        nodes.push_back({s_node_position.get<0>(), s_node_position.get<1>(), s_node_position.get<2>()});
        nodes.push_back({e_node_position.get<0>(), e_node_position.get<1>(), e_node_position.get<2>()});

        REQUIRE(std::is_permutation(nodes.begin(), nodes.end(), expected_nodes.begin()));
    }

    SECTION("Via segment properties")
    {
        auto net_N3 = netlist.find_net("N3");
        auto segments = global_routing.segments(net_N3);
        auto box = box_type{point_type{unit_type{10}, unit_type{10}}, point_type{unit_type{20}, unit_type{20}}};
        auto segment_it = std::find_if(segments.begin(),
                                       segments.end(),
                                       [&](auto & seg){return boxComparator(global_routing.box(seg), box);});

        auto start_layer = global_routing.layer_start(*segment_it);
        auto end_layer = global_routing.layer_end(*segment_it);
        REQUIRE(start_layer != end_layer);
        auto start_gcell = global_routing.gcell_start(*segment_it);
        auto end_gcell = global_routing.gcell_end(*segment_it);

        auto s_graph_node = graph_ptr->graph_node(start_gcell);
        auto s_node_position = graph_ptr->position(s_graph_node);

        auto e_graph_node = graph_ptr->graph_node(end_gcell);
        auto e_node_position = graph_ptr->position(e_graph_node);

        std::vector<std::tuple<int,int,int>> expected_nodes={{1,1,0}, {1,1,2}};
        std::vector<std::tuple<int,int,int>> nodes;
        nodes.push_back({s_node_position.get<0>(), s_node_position.get<1>(), s_node_position.get<2>()});
        nodes.push_back({e_node_position.get<0>(), e_node_position.get<1>(), e_node_position.get<2>()});

        REQUIRE(std::is_permutation(nodes.begin(), nodes.end(), expected_nodes.begin()));
    }

    SECTION("GCell Graph demand"){
        //initial demand = 0
        int all_demand = 0;
        for(int z = 0; z < graph_ptr->depth(); z++)
            for(int x = 0; x < graph_ptr->width(); x++)
                for(int y = 0; y < graph_ptr->width(); y++)
                    all_demand += graph_ptr->demand(graph_ptr->gcell(x,y,z));
        REQUIRE(all_demand == 0);

        //add net N3 demand
        auto net_N3 = netlist.find_net("N3");
        global_routing.increase_demand(net_N3);
        for(int z = 0; z < graph_ptr->depth(); z++)
            for(int x = 0; x < graph_ptr->width(); x++)
                for(int y = 0; y < graph_ptr->width(); y++)
                    all_demand += graph_ptr->demand(graph_ptr->gcell(x,y,z));
        REQUIRE(all_demand == 10);

        //remove net N3 demand
        all_demand = 0;
        global_routing.decrease_demand(net_N3);
        for(int z = 0; z < graph_ptr->depth(); z++)
            for(int x = 0; x < graph_ptr->width(); x++)
                for(int y = 0; y < graph_ptr->width(); y++)
                    all_demand += graph_ptr->demand(graph_ptr->gcell(x,y,z));
        REQUIRE(all_demand == 0);
    }
}
