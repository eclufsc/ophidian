#include <catch.hpp>

#include <ophidian/entity_system/entity_system.h>
#include <ophidian/entity_system/property.h>

#include "entity_system_fixture.h"


using namespace entity_system_test;

TEST_CASE("entity: null", "[entity_system]")
{
    entity_system::entity en;
    REQUIRE(en.id() == entity_system::entity::invalid());
    REQUIRE(en == entity_system::entity::null());
}

TEST_CASE_METHOD(empty_system, "entity_system: entity system has a constructor", "[entity_system]") {
    REQUIRE(sys.size() == 0);
}

TEST_CASE_METHOD(empty_system, "entity_system: can't destroy null", "[entity_system]") {
    REQUIRE_THROWS(sys.destroy(entity_system::entity::null()));
}

TEST_CASE_METHOD(system_with_1_entity, "entity_system: add entity", "[entity_system]") {
    REQUIRE(entity<0>().id() == 0);
    REQUIRE(sys.size() == 1);
}

TEST_CASE_METHOD(system_with_1_entity, "entity_system: check entity", "[entity_system]") {
    REQUIRE(sys.valid(entity<0>()));
    REQUIRE(!sys.valid(entity_system::entity::null()));
}

TEST_CASE_METHOD(destroy_first_entity<system_with_1_entity>, "entity_system: destroy entity", "[entity_system]") {
    REQUIRE(sys.size() == 0);
    REQUIRE(!sys.valid(entity<0>()));
}

TEST_CASE_METHOD(destroy_first_entity<system_with_1_entity>, "entity_system: can't destroy twice", "[entity_system]") {
    REQUIRE_THROWS(sys.destroy(entity<0>()));
}

TEST_CASE_METHOD(system_with_1_entity, "entity_system: lookup", "[entity_system]") {
    REQUIRE( sys.lookup(entity<0>()) == 0 );
    REQUIRE_THROWS(sys.lookup(entity_system::entity::null()));
}

TEST_CASE_METHOD(system_with_2_entities, "entity_system: system with two entities", "[entity_system]") {
    REQUIRE(sys.size() == 2);
    REQUIRE( sys.lookup(entity<0>()) == 0 );
    REQUIRE( sys.lookup(entity<1>()) == 1 );

}

TEST_CASE_METHOD(system_with_2_entities, "entity_system: crescent ids", "[entity_system]") {
    REQUIRE(entity<0>().id() == 0);
    REQUIRE(entity<1>().id() == 1);
}

TEST_CASE_METHOD(destroy_first_entity<system_with_2_entities>, "entity_system: destroy first, keep second", "[entity_system]") {
    REQUIRE(!sys.valid(entity<0>()));
    REQUIRE(sys.valid(entity<1>()));
    REQUIRE(sys.size() == 1);
    REQUIRE_THROWS( sys.lookup(entity<0>()) );
    REQUIRE( sys.lookup(entity<1>()) == 0 );
}


TEST_CASE_METHOD(system_with_2_entities, "entity_system: clear", "[entity_system]") {
    sys.clear();
    REQUIRE(sys.size() == 0);
}

TEST_CASE_METHOD(system_with_3_entities, "entity_system: system with three entities", "[entity_system]") {
    REQUIRE(sys.size() == 3);
}

TEST_CASE_METHOD(destroy_first_entity<system_with_3_entities>, "entity_system: destroy first, keep second and third", "[entity_system]") {
    REQUIRE(!sys.valid(entity<0>()));
    REQUIRE(sys.valid(entity<1>()));
    REQUIRE(sys.valid(entity<2>()));
    REQUIRE(sys.size() == 2);
}

TEST_CASE_METHOD(empty_system, "entity_system: notifier without prop", "[entity_system]") {
    REQUIRE( !sys.has_property(prop) );
    REQUIRE( sys.properties_size() == 0);
}

TEST_CASE_METHOD(attatched_empty_system, "entity_system: notifier attach", "[entity_system]") {
    REQUIRE( sys.has_property(prop) );
    REQUIRE( sys.properties_size() == 1);
}

TEST_CASE_METHOD(attatched_empty_system, "entity_system: notifier detach", "[entity_system]") {
    sys.notifier()->dettach(prop);
    REQUIRE( sys.properties_size() == 0);
    REQUIRE(!sys.has_property(prop));
}


TEST_CASE_METHOD(empty_system_with_int_property, "entity_system: property<int>", "[entity_system]") {
    REQUIRE(ints.size() == 0);
    REQUIRE( sys.properties_size() == 1);
    REQUIRE(sys.has_property(ints));
}

TEST_CASE_METHOD(empty_system_with_int_property, "entity_system: add entity, add property", "[entity_system]") {
    entity_system::entity created = sys.create();
    REQUIRE(ints.size() == 1);
}

TEST_CASE_METHOD(system_with_3_entities_and_a_property, "entity_system: create property<int> after create entities", "[entity_system]") {
    REQUIRE(prop.size() == 3);
}


TEST_CASE_METHOD(system_with_3_entities_and_a_property, "entity_system: get property value", "[entity_system]") {
    REQUIRE( prop[entity<0>()] == 1 );
    REQUIRE( prop[entity<1>()] == 2 );
    REQUIRE( prop[entity<2>()] == 3 );
}


TEST_CASE_METHOD(system_with_3_entities_and_a_property, "entity_system: assign property value", "[entity_system]") {
    prop[entity<1>()] = 6;
    REQUIRE( prop[entity<0>()] == 1 );
    REQUIRE( prop[entity<1>()] == 6 );
    REQUIRE( prop[entity<2>()] == 3 );
}

TEST_CASE_METHOD(destroy_first_entity<system_with_3_entities_and_a_property>, "entity_system: keep property values of others when destroying", "[entity_system]") {
    REQUIRE( fixture.prop[entity<1>()] == 2 );
    REQUIRE( fixture.prop[entity<2>()] == 3 );
}

TEST_CASE_METHOD(destroy_first_entity<system_with_3_entities_and_a_property>, "entity_system: try to retrieve property from destroyed entity", "[entity_system]") {
    REQUIRE_THROWS( fixture.prop[entity<0>()] );
    REQUIRE(fixture.prop.size() == 2);
}

TEST_CASE_METHOD(system_with_3_entities_and_a_property, "entity_system: clear property after clearing system", "[entity_system]") {
    sys.clear();
    REQUIRE( prop.size() == 0 );
}

//TEST_CASE("entity_system: independent lifetime", "[entity_system]") {
//    entity_system *sys = new entity_system;
//    property<int> ints(*sys);
//    sys->create();
//    sys->create();
//    delete sys;
//    REQUIRE( ints.size() == 2 );
//}


TEST_CASE_METHOD(system_with_4_entities, "entity_system: helper create N entities", "[entity_system]") {
    REQUIRE( en.size() == 4 );
    REQUIRE( sys.valid(entity<0>()));
    REQUIRE( sys.valid(entity<1>()));
    REQUIRE( sys.valid(entity<2>()));
    REQUIRE( sys.valid(entity<3>()));
}

TEST_CASE_METHOD(system_with_4_entities, "entity_system: entity system bounds", "[entity_system]") {
    REQUIRE( std::distance(sys.begin(), sys.end()) == 4 );
}


TEST_CASE("entity_system: create property", "[entity_system]") {
    entity_system sys;
    entity_system::entity en = sys.create();
    auto ids = make_property<int>(sys);
    REQUIRE_NOTHROW(ids[en] = 2);
    REQUIRE(ids[en] == 2);
}

TEST_CASE("entity_system: null", "[entity_system]") {
    REQUIRE( entity_system::null().properties_size() == 0);
    property_mock * prop;
    prop = new property_mock;
    REQUIRE( entity_system::null().properties_size() == 1);
    REQUIRE( entity_system::null().has_property(*prop) );

    delete prop;
    REQUIRE( entity_system::null().properties_size() == 0);

}
