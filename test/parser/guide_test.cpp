#include <catch.hpp>
#include <regex>

#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>

using ophidian::parser::Guide;
using dbu_t = Guide::database_unit_type;

TEST_CASE("Guide: Try to load inexistent file", "[parser][Guide]")
{
    CHECK_THROWS_AS(
        Guide{"a_file_with_this_name_should_not_exist"},
        ophidian::parser::exceptions::InexistentFile
    );
}

TEST_CASE("Guide: Loading ispd18_sample.input.guide", "[parser][Guide][sample]")
{
    Guide sample = Guide{"input_files/ispd18/ispd18_sample/ispd18_sample.input.guide"};

    CHECK(sample.nets().size() == 11);
    auto& nets = sample.nets();

    auto& net = nets[0];
    CHECK(net.name() == "net1230");
    CHECK(net.regions().size() == 5);

    auto& regions = net.regions();
    CHECK(regions[0].geometry().min_corner().x() == dbu_t{89600});
    CHECK(regions[0].geometry().min_corner().y() == dbu_t{71820});
    CHECK(regions[0].geometry().max_corner().x() == dbu_t{95600});
    CHECK(regions[0].geometry().max_corner().y() == dbu_t{77520});
    CHECK(regions[0].metal_layer_name() == "Metal1");

    CHECK(regions[1].geometry().min_corner().x() == dbu_t{95600});
    CHECK(regions[1].geometry().min_corner().y() == dbu_t{83220});
    CHECK(regions[1].geometry().max_corner().x() == dbu_t{104400});
    CHECK(regions[1].geometry().max_corner().y() == dbu_t{91200});
    CHECK(regions[1].metal_layer_name() == "Metal1");

    CHECK(regions[2].geometry().min_corner().x() == dbu_t{89600});
    CHECK(regions[2].geometry().min_corner().y() == dbu_t{71820});
    CHECK(regions[2].geometry().max_corner().x() == dbu_t{95600});
    CHECK(regions[2].geometry().max_corner().y() == dbu_t{83220});
    CHECK(regions[2].metal_layer_name() == "Metal2");

    CHECK(regions[3].geometry().min_corner().x() == dbu_t{95600});
    CHECK(regions[3].geometry().min_corner().y() == dbu_t{77520});
    CHECK(regions[3].geometry().max_corner().x() == dbu_t{104400});
    CHECK(regions[3].geometry().max_corner().y() == dbu_t{91200});
    CHECK(regions[3].metal_layer_name() == "Metal2");

    CHECK(regions[4].geometry().min_corner().x() == dbu_t{89600});
    CHECK(regions[4].geometry().min_corner().y() == dbu_t{77520});
    CHECK(regions[4].geometry().max_corner().x() == dbu_t{104400});
    CHECK(regions[4].geometry().max_corner().y() == dbu_t{83220});
    CHECK(regions[4].metal_layer_name() == "Metal3");
}
