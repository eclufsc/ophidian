#include "property_test.h"
#include <catch.hpp>

#include <ophidian/entity_system/Property.h>

using namespace ophidian::entity_system;

MyEntity::MyEntity() : EntityBase()
{}

MyEntity::MyEntity(uint32_t id, EntitySystemBase *system) :
    EntityBase(id, system)
{}

class MyDummyProperty {
public:
    MyDummyProperty() {}
};

TEST_CASE("Property: null property", "[entity_system][Property]")
{
    Property<MyEntity, MyDummyProperty> prop;
    REQUIRE( prop.empty() );
    REQUIRE( prop.size() == 0 );
}


TEST_CASE("Property: constructor before creating entities", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   Property<MyEntity, MyDummyProperty> prop(sys);
   REQUIRE( prop.size() == 0 );
   sys.add();
   REQUIRE( prop.size() == 1 );
}

TEST_CASE("Property: constructor after creating entities", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   sys.add();
   Property<MyEntity, MyDummyProperty> prop(sys);
   REQUIRE( prop.size() == 1 );
}

TEST_CASE("Property: assignment operator", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   Property<MyEntity, MyDummyProperty> prop;
   // ...
   prop = Property<MyEntity, MyDummyProperty>(sys);
   sys.add();
   REQUIRE( prop.size() == 1 );
}


TEST_CASE("Property: lazy assignment operator", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   Property<MyEntity, MyDummyProperty> prop;
   sys.add();
   prop = Property<MyEntity, MyDummyProperty>(sys);
   REQUIRE( prop.size() == 1 );
}

TEST_CASE("Property: move constructor", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   Property<MyEntity, MyDummyProperty> prop = Property<MyEntity, MyDummyProperty>(sys);
   REQUIRE( prop.size() == 0 );
   sys.add();
   REQUIRE( prop.size() == 1 );
}

TEST_CASE("Property: move constructor after creating entities", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   sys.add();
   Property<MyEntity, MyDummyProperty> prop = Property<MyEntity, MyDummyProperty>(sys);
   REQUIRE( prop.size() == 1 );
}


TEST_CASE("Property: subscript operator", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   auto entity = sys.add();
   Property<MyEntity, int> prop(sys);
   prop[entity] = 3;
   REQUIRE( prop[entity] == 3 );
   prop[entity] = 2;
   REQUIRE( prop[entity] == 2 );
}

TEST_CASE("Property: const subscript operator", "[entity_system][Property]")
{
   EntitySystem<MyEntity> sys;
   auto entity = sys.add();
   Property<MyEntity, int> prop(sys);
   prop[entity] = 3;

   auto funct = [entity](const Property<MyEntity, int> & prop){
        auto element = prop[entity];
//        auto& element = prop[entity]; -> doesn't compile!!
        element = 0;
   };
   funct(prop);
   REQUIRE( prop[entity] == 3 );
}

TEST_CASE("Property: subscript operator (3 elements)", "[entity_system][Property]") {
    EntitySystem<MyEntity> sys;
    auto en1 = sys.add();
    auto en2 = sys.add();
    auto en3 = sys.add();

    Property<MyEntity, int> prop(sys);
    prop[en1] = 0;
    prop[en2] = 1;
    prop[en3] = 2;

    REQUIRE( prop[en1] == 0 );
    REQUIRE( prop[en2] == 1 );
    REQUIRE( prop[en3] == 2 );

    prop[en2] = 4;

    REQUIRE( prop[en1] == 0 );
    REQUIRE( prop[en2] == 4 );
    REQUIRE( prop[en3] == 2 );
}


TEST_CASE("Property: reserve, capacity & shrink", "[entity_system][Property]") {
    EntitySystem<MyEntity> sys;
    auto en1 = sys.add();
    auto en2 = sys.add();
    auto en3 = sys.add();

    Property<MyEntity, int> prop(sys);
    prop.reserve(42);
    REQUIRE( prop.capacity() == 42 );
    prop.shrink();
    REQUIRE( prop.capacity() == 3 );

}

TEST_CASE("Property: property capacity", "[entity_system][Property]") {
    EntitySystem<MyEntity> sys;
    sys.reserve(42);
    Property<MyEntity, int> prop(sys);
    sys.add();
    sys.add();
    REQUIRE( prop.capacity() == 42 );
    sys.shrink();
    REQUIRE( prop.capacity() == 2 );

}



