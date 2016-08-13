#include <catch.hpp>

#include <ophidian/entity_system/property.h>

#include "entity_system_fixture.h"

using namespace ophidian::entity_system;

TEST_CASE("property: constructor & destructor", "[property]"){
    property<int> * prop;
    REQUIRE_NOTHROW(prop = new property<int>);
    REQUIRE_NOTHROW(delete prop);
    REQUIRE(entity_system::null().properties_size() == 0);
}

TEST_CASE("property: assignment", "[property]"){
    entity_system sys;
    make_entities<10>(sys);
    property<int> prop{make_property<int>(sys)};
    property<int> prop2;
    prop2 = prop;

    REQUIRE( sys.properties_size() == 2 );
    REQUIRE( prop2.system() == prop.system() );
    REQUIRE( !entity_system::null().has_property(prop) );
    REQUIRE( !entity_system::null().has_property(prop2) );
    REQUIRE( entity_system::null().properties_size() == 0 );
    REQUIRE( sys.has_property(prop) );
    REQUIRE( sys.has_property(prop2) );
    REQUIRE( prop.size() == 10 );
    REQUIRE( prop2.size() == 10 );

    auto last_created = sys.create();

    REQUIRE(prop.size() == 11);
    REQUIRE(prop2.size() == 11);

    sys.destroy(last_created);

    REQUIRE( prop.size() == 10 );
    REQUIRE( prop2.size() == 10 );

    sys.clear();

    REQUIRE( prop.size() == 0 );
    REQUIRE( prop2.size() == 0 );
}



TEST_CASE("property: can't access", "[property]") {
    property<int> prop;
    entity_system::entity en;
    REQUIRE_THROWS(prop[en]);
    REQUIRE(entity_system::null().has_property(prop));
    REQUIRE(prop.system() == entity_system::null());
}

TEST_CASE("property: set entity system", "[property]") {
    entity_system sys1;
    property<int> prop;
    prop.system(sys1);
    REQUIRE(!entity_system::null().has_property(prop));
    REQUIRE(entity_system::null().properties_size() == 0);
    REQUIRE(sys1.has_property(prop));
    REQUIRE(sys1.properties_size() == 1);
    REQUIRE(prop.system() == sys1);
}

TEST_CASE_METHOD(entity_system_test::empty_system, "property: is attached to system", "[property]") {
    property<int> prop{make_property<int>(sys)};
    REQUIRE(prop.system() == sys);
    REQUIRE(sys.has_property(prop));
}

TEST_CASE_METHOD(entity_system_test::empty_system, "property: dettach property from system", "[property]") {
    property<int> prop{make_property<int>(sys)};
    prop.system(entity_system::null());
    REQUIRE(entity_system::null().has_property(prop));
    REQUIRE(prop.system() == entity_system::null());
    REQUIRE(!sys.has_property(prop));
    REQUIRE(sys.properties_size() == 0);
}

TEST_CASE_METHOD(entity_system_test::empty_system, "property: helper to create properties", "[property]") {
    property<int> prop_1;
    property<int> prop_2{make_property<int>(sys)};
    auto en = sys.create();
    REQUIRE(sys.has_property(prop_2));
    REQUIRE(!sys.has_property(prop_1));
    REQUIRE(entity_system::null().has_property(prop_1));
    REQUIRE_NOTHROW(prop_2[en] = 2);
    REQUIRE(prop_2[en] == 2);
    REQUIRE_NOTHROW( prop_2[en] );
}



TEST_CASE_METHOD(entity_system_test::system_with_1_entity, "property<bool>", "[property]") {
    property<bool> prop(make_property<bool>(sys));
    prop[entity<0>()] = false;
    REQUIRE( !prop[entity<0>()] );
    prop[entity<0>()] = true;
    REQUIRE( prop[entity<0>()] );

}

