#include "aggregation_test.h"
#include <catch.hpp>

#include <ophidian/entity_system/Aggregation.h>

using namespace ophidian::entity_system;

class EntityA : public EntityBase
{
    public:
        using EntityBase::EntityBase;
};

class EntityB : public EntityBase
{
    public:
        using EntityBase::EntityBase;
};

TEST_CASE("Aggregation: no parts", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto parts = aggregation.parts(en1);
    REQUIRE(parts.begin() == parts.end());
    REQUIRE(parts.empty());
    REQUIRE(parts.size() == 0);
    REQUIRE(aggregation.firstPart(en1) == EntityB());
}


TEST_CASE("Aggregation: add part", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto en2 = sys2.add();
    REQUIRE(aggregation.whole(en2) == EntityA());
    aggregation.addAssociation(en1, en2);
    REQUIRE(aggregation.whole(en2) == en1);
    auto parts = aggregation.parts(en1);
    REQUIRE(parts.begin() != parts.end());
    REQUIRE(!parts.empty());
    REQUIRE(parts.size() == 1);
    REQUIRE(std::count(parts.begin(), parts.end(), en2) == 1);
}




TEST_CASE("Aggregation: erase part", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto en2 = sys2.add();
    aggregation.addAssociation(en1, en2);
    sys2.erase(en2);
    auto parts = aggregation.parts(en1);
    REQUIRE(std::count(parts.begin(), parts.end(), en2) == 0);
}

TEST_CASE("Aggregation: erase whole", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto en2 = sys2.add();
    aggregation.addAssociation(en1, en2);
    sys1.erase(en1);
    REQUIRE(aggregation.whole(en2) == EntityA());
}

TEST_CASE("Aggregation: add parts, erase one, keep others", "[entity_system][Property][Aggregation][EntitySystem]")
{
    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    auto en1 = sys1.add();
    auto part1 = sys2.add();
    auto part2 = sys2.add();
    auto part3 = sys2.add();
    auto part4 = sys2.add();
    auto part5 = sys2.add();
    aggregation.addAssociation(en1, part1);
    aggregation.addAssociation(en1, part2);
    aggregation.addAssociation(en1, part3);
    aggregation.addAssociation(en1, part4);
    aggregation.addAssociation(en1, part5);
    sys2.erase(part4);
    REQUIRE(aggregation.whole(part1) == en1);
    REQUIRE(aggregation.whole(part2) == en1);
    REQUIRE(aggregation.whole(part3) == en1);
    REQUIRE(aggregation.whole(part5) == en1);
    REQUIRE(aggregation.parts(en1).size() == 4);
}


TEST_CASE("Aggregation: clear()", "[entity_system][Property][Aggregation][EntitySystem]")
{
    INFO("Given an aggregation with 5 wholes and some parts");

    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;
    Aggregation<EntityA, EntityB> aggregation(sys1, sys2);
    std::vector<EntityA> wholes{ sys1.add(), sys1.add(), sys1.add(), sys1.add(), sys1.add() };
    aggregation.addAssociation(wholes[0], sys2.add());
    aggregation.addAssociation(wholes[0], sys2.add());

    aggregation.addAssociation(wholes[1], sys2.add());
    aggregation.addAssociation(wholes[1], sys2.add());
    aggregation.addAssociation(wholes[1], sys2.add());

    aggregation.addAssociation(wholes[2], sys2.add());

    aggregation.addAssociation(wholes[3], sys2.add());
    aggregation.addAssociation(wholes[3], sys2.add());
    aggregation.addAssociation(wholes[3], sys2.add());
    aggregation.addAssociation(wholes[3], sys2.add());

    aggregation.addAssociation(wholes[4], sys2.add());
    aggregation.addAssociation(wholes[4], sys2.add());

    INFO("When the wholes system is cleared");

    sys1.clear();

    INFO("Then parts must be kept valid")

    REQUIRE(sys2.size() == 12);


    INFO("Then all parts must have nextPart == NIL")
    {
        std::list<EntityB> partsWithNextPart;
        std::copy_if(sys2.begin(), sys2.end(), std::back_inserter(partsWithNextPart), [&aggregation](const EntityB & en)->bool
        {
            return aggregation.nextPart(en) != EntityB();
        });
        REQUIRE(partsWithNextPart.empty());
    }

    INFO("Then all parts must have whole == NIL")
    {
        std::list<EntityB> partsWithWhole;
        std::copy_if(sys2.begin(), sys2.end(), std::back_inserter(partsWithWhole), [&aggregation](const EntityB & en)->bool
        {
            return aggregation.whole(en) != EntityA();
        });
        REQUIRE(partsWithWhole.empty());
    }
}

TEST_CASE("Aggregation: lifetime managment", "[entity_system][Property][Aggregation][EntitySystem]")
{
    std::unique_ptr<Aggregation<EntityA, EntityB> > agg;

    EntitySystem<EntityA> sys1;
    EntitySystem<EntityB> sys2;

    REQUIRE_NOTHROW(sys1.add());

    agg = std::move(std::make_unique<Aggregation<EntityA, EntityB> >(sys1, sys2));

    REQUIRE_NOTHROW(sys1.add());

    agg.reset();

    REQUIRE_NOTHROW(sys1.add());
}
