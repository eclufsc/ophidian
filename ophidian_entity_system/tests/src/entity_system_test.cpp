#include <catch2/catch.hpp>

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>

using namespace ophidian::entity_system;

using Entity = EntityBase;

TEST_CASE("EntitySystem: empty", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    REQUIRE( sys.empty() );
    REQUIRE( sys.size() == 0 );
    REQUIRE( sys.capacity() == 0 );
}

TEST_CASE("EntitySystem: add entity", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    auto entity = sys.add();
    REQUIRE( sys.size() == 1 );
    REQUIRE( sys.valid(entity) );
    REQUIRE( std::count(sys.begin(), sys.end(), entity) == 1 );
}

TEST_CASE("EntitySystem: erase entity", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    auto entity = sys.add();
    sys.erase(entity);
    REQUIRE( sys.size() == 0 );
    REQUIRE( !sys.valid(entity) );
    REQUIRE( sys.empty() );
    REQUIRE( std::count(sys.begin(), sys.end(), entity) == 0 );
}

TEST_CASE("EntitySystem: capacity", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    sys.reserve(10000);
    REQUIRE(sys.capacity() == 10000);
}

TEST_CASE("EntitySystem: erase entity 2", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    auto entity1 = sys.add();
    auto entity2 = sys.add();
    auto entity3 = sys.add();
    sys.erase(entity2);
    REQUIRE( sys.size() == 2 );
    REQUIRE( std::count(sys.begin(), sys.end(), entity1) == 1 );
    REQUIRE( std::count(sys.begin(), sys.end(), entity2) == 0 );
    REQUIRE( std::count(sys.begin(), sys.end(), entity3) == 1 );
}

TEST_CASE("EntitySystem: clear", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    auto entity1 = sys.add();
    auto entity2 = sys.add();
    auto entity3 = sys.add();
    sys.clear();
    REQUIRE( sys.size() == 0 );
    REQUIRE( sys.empty() );
}


TEST_CASE("EntitySystem: shrink", "[entity_system][EntitySystem]") {
    EntitySystem<Entity> sys;
    sys.reserve(35);
    sys.add();
    sys.add();
    sys.add();
    sys.shrinkToFit();
    REQUIRE( sys.capacity() == 3 );
}
