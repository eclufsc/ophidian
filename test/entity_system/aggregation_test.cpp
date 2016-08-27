#include "aggregation_test.h"
#include <catch.hpp>

#include <ophidian/entity_system/Aggregation.h>

using namespace ophidian::entity_system;

class EntityA : public EntityBase {
public:
    EntityA(uint32_t id, EntitySystemBase* system) :
        EntityBase(id, system)
    {

    }
    EntityA() : EntityBase() {

    }
};

class EntityB : public EntityBase {
public:
    EntityB(uint32_t id, EntitySystemBase* system) :
        EntityBase(id, system)
    {

    }
};

TEST_CASE("Aggregation: erase part", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto en2 = sys2.add();
    aggregation.addPart(en1, en2);

    sys2.erase(en2);
    REQUIRE( aggregation.parts(en1).empty() );
}

TEST_CASE("Aggregation: erase whole", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto en2 = sys2.add();
    aggregation.addPart(en1, en2);
    sys1.erase(en1);
    REQUIRE( aggregation.whole(en2) == EntityA() );
}
