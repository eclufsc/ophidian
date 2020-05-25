#include <catch2/catch.hpp>
#include <ophidian/util/GridGraph_3D.h>
#include <ophidian/util/Units.h>
#include <ophidian/util/UtilException.h>

// using namespace ophidian::parser;

TEST_CASE("Grid Graph 3D Node Test", "[util][graph]")
{
    //creating a 3x3x3 graph
    auto graph = ophidian::util::GridGraph_3D(3,3,3,1,1);

    SECTION("Try to get node outside graph limits"){
        CHECK(graph.node(5,5,5) == lemon::INVALID);
    }

    SECTION("Try to get node outside highest limit on x"){
        CHECK(graph.east_node(2,1,1) == lemon::INVALID);
    }

    SECTION("Try to get node outside highest limit on y"){
        CHECK(graph.north_node(1,2,1) == lemon::INVALID);
    }

    SECTION("Try to get node outside highest limit on z"){
        CHECK(graph.up_node(1,1,2) == lemon::INVALID);
    }

    SECTION("Try to get node outside lowest limit on x"){
        CHECK(graph.west_node(0,1,1) == lemon::INVALID);
    }

    SECTION("Try to get node outside lowest limit on y"){
        CHECK(graph.south_node(1,0,1) == lemon::INVALID);
    }

    SECTION("Try to get node outside lowest limit on z"){
        CHECK(graph.down_node(1,1,0) == lemon::INVALID);
    }

    SECTION("get a valid node"){
        auto node = graph.node(2,1,0);
        auto position = graph.position(node);
        CHECK(position.get<0>() == 2);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 0);
    }

    //center node of cube 3x3x3
    auto node = graph.node(1,1,1);

    SECTION("get north neighbor of a valid node"){
        auto neighbor = graph.north_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 2);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get north neighbor of a valid node with coordinates"){
        auto neighbor = graph.north_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 2);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get south neighbor of a valid node"){
        auto neighbor = graph.south_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 0);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get south neighbor of a valid node with coordinates"){
        auto neighbor = graph.south_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 0);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get east neighbor of a valid node"){
        auto neighbor = graph.east_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 2);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get east neighbor of a valid node with coordinates"){
        auto neighbor = graph.east_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 2);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get west neighbor of a valid node"){
        auto neighbor = graph.west_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 0);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get west neighbor of a valid node with coordinates"){
        auto neighbor = graph.west_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 0);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 1);
    }

    SECTION("get upper neighbor of a valid node"){
        auto neighbor = graph.up_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 2);
    }

    SECTION("get upper neighbor of a valid node with coordinates"){
        auto neighbor = graph.up_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 2);
    }

    SECTION("get down neighbor of a valid node"){
        auto neighbor = graph.down_node(node);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 0);
    }

    SECTION("get down neighbor of a valid node with coordinates"){
        auto neighbor = graph.down_node(1,1,1);
        auto position = graph.position(neighbor);
        CHECK(position.get<0>() == 1);
        CHECK(position.get<1>() == 1);
        CHECK(position.get<2>() == 0);
    }
}

TEST_CASE("Routing Graph Edge Test", "[util][graph]")
{
    //creating a 3x3x3 graph
    auto graph = ophidian::util::GridGraph_3D(3,3,3,1,1);

    SECTION("Try to get edge outside highest limit on x"){
        CHECK(graph.east_edge(2,1,1) == lemon::INVALID);
    }

    SECTION("Try to get edge outside highest limit on y"){
        CHECK(graph.north_edge(1,2,1) == lemon::INVALID);
    }

    SECTION("Try to get edge outside highest limit on z"){
        CHECK(graph.up_edge(1,1,2) == lemon::INVALID);
    }

    SECTION("Try to get edge outside lowest limit on x"){
        CHECK(graph.west_edge(0,1,1) == lemon::INVALID);
    }

    SECTION("Try to get edge outside lowest limit on y"){
        CHECK(graph.south_edge(1,0,1) == lemon::INVALID);
    }

    SECTION("Try to get edge outside lowest limit on z"){
        CHECK(graph.down_edge(1,1,0) == lemon::INVALID);
    }

    //center node of cube 3x3x3
    auto node = graph.node(1,1,1);

    SECTION("Verify edge capacity"){
        auto target = graph.north_node(node);
        auto edge = graph.edge(node, target);
        auto capacity = graph.capacity(edge);
        CHECK(capacity == 1);
    }

    SECTION("Verify north edge"){
        auto target = graph.north_node(node);
        auto edge = graph.north_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }

    SECTION("Verify south edge"){
        auto target = graph.south_node(node);
        auto edge = graph.south_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }

    SECTION("Verify east edge"){
        auto target = graph.east_node(node);
        auto edge = graph.east_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }

    SECTION("Verify west edge"){
        auto target = graph.west_node(node);
        auto edge = graph.west_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }

    SECTION("Verify upper edge"){
        auto target = graph.up_node(node);
        auto edge = graph.up_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }

    SECTION("Verify down edge"){
        auto target = graph.down_node(node);
        auto edge = graph.down_edge(node);

        auto u = graph.u(edge);
        auto v = graph.v(edge);
        CHECK(((node == u && target == v) || (node == v && target == u)));
    }
}
