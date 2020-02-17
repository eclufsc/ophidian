#include <catch.hpp>
#include <vector>

#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/GlobalRoutingFactory.h>
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
        auto layer_instance = globalRouting.layer(part);
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

