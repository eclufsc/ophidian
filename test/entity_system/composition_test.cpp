#include <catch.hpp>

#include <ophidian/entity_system/composition.h>

#include "association_fixture.h"

using namespace ophidian::entity_system;

using whole_composed_of_three_parts_fixture = association_fixture::whole_composed_of_three_parts_fixture_plus_one_unattached_part<composition>;

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: destroy whole, destroy parts", "[composition]") {
    composite().destroy(whole());
    REQUIRE( !component().valid(part<0>()) );
    REQUIRE( !component().valid(part<1>()) );
    REQUIRE( !component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: clear whole system, destroy parts", "[composition]") {
    composite().clear();
    REQUIRE( !component().valid(part<0>()) );
    REQUIRE( !component().valid(part<1>()) );
    REQUIRE( !component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
}


TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: destroy part, remove from whole", "[composition]") {
    component().destroy(part<0>());
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE( relation().is_component_of(whole(), part<1>()) );
    REQUIRE( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 2 );
    REQUIRE( relation().composite_of(part<1>()) == whole() );
    REQUIRE( relation().composite_of(part<2>()) == whole() );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: clear parts, remove from wholes", "[composition]") {
    component().clear();
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<1>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 0 );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}


struct self_composition_with_root_and_child_fixture {
    entity_system sys;
    composition compo{sys, sys};
    entity_system::entity root;
    entity_system::entity child;

    self_composition_with_root_and_child_fixture() :
        root(sys.create()),
        child(sys.create())
    {
        compo.attach_component(root, child);
    }
};


TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition:  'self_composition_with_root_and_child_fixture' test", "[composition]") {
    REQUIRE( compo.components_size(root) == 1 );
    REQUIRE( compo.composite_of(child) == root );
    REQUIRE( std::find(compo.components_bounds(root).begin(), compo.components_bounds(root).end(), child) != compo.components_bounds(root).end() );
}


TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition: self composition, attach child, destroy parent", "[composition]") {
    sys.destroy(root);
    REQUIRE( !sys.valid(child) );
    REQUIRE_THROWS( compo.components_size(root) );
    REQUIRE_THROWS( compo.components_size(child) );
}

TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition: self composition, attach child, destroy child", "[composition]") {
    sys.destroy(child);
    REQUIRE( compo.components_size(root) == 0 );
    REQUIRE_THROWS( compo.components_size(child) );
}


TEST_CASE("composition: skeleton example", "[composition]") {
    entity_system skeleton;
    composition joints(skeleton, skeleton);

    auto spine = skeleton.create();
    auto skull = skeleton.create();
    joints.attach_component(spine, skull);

    const int RIGHT = 0;
    const int LEFT = 1;

    std::array<entity_system::entity, 2> collerball, arm1, arm2, hand, femur, lower_leg, foot;
    std::array<std::array<entity_system::entity, 5>, 2> fingers, toes;
    std::array<std::array<entity_system::entity, 12>, 2> ribs;

    for(std::size_t i = 0; i < 2; ++i) // left and right sides
    {
        collerball[i] = skeleton.create();
        joints.attach_component(spine, collerball[i]);

        arm1[i] = skeleton.create();
        joints.attach_component(collerball[i], arm1[i]);

        arm2[i] = skeleton.create();
        joints.attach_component(arm1[i], arm2[i]);

        hand[i] = skeleton.create();
        joints.attach_component(arm2[i], hand[i]);

        fingers[i] = make_entities<5>(skeleton);
        attach_component(joints, hand[i], fingers[i].begin(), fingers[i].end());

        femur[i] = skeleton.create();
        joints.attach_component(spine, femur[i]);

        lower_leg[i] = skeleton.create();
        joints.attach_component(femur[i], lower_leg[i]);

        foot[i] = skeleton.create();
        joints.attach_component(lower_leg[i], foot[i]);

        toes[i] = make_entities<5>(skeleton);
        attach_component(joints, foot[i], toes[i].begin(), toes[i].end());

        ribs[i] = make_entities<12>(skeleton);
        attach_component(joints, spine, ribs[i].begin(), ribs[i].end());
    }

    skeleton.destroy(arm1[RIGHT]);

    REQUIRE( skeleton.valid(skull) );
    REQUIRE( skeleton.valid(spine) );

    REQUIRE( skeleton.valid(collerball[RIGHT]) );
    REQUIRE( !skeleton.valid(arm2[RIGHT]) );
    REQUIRE( !skeleton.valid(arm1[RIGHT]) );
    REQUIRE( !skeleton.valid(hand[RIGHT]) );
    REQUIRE( !skeleton.valid(fingers[RIGHT].at(0)) );
    REQUIRE( !skeleton.valid(fingers[RIGHT].at(1)) );
    REQUIRE( !skeleton.valid(fingers[RIGHT].at(2)) );
    REQUIRE( !skeleton.valid(fingers[RIGHT].at(3)) );
    REQUIRE( !skeleton.valid(fingers[RIGHT].at(4)) );
    REQUIRE( skeleton.valid(femur[RIGHT]) );
    REQUIRE( skeleton.valid(lower_leg[RIGHT]) );
    REQUIRE( skeleton.valid(foot[RIGHT]) );
    REQUIRE( skeleton.valid(toes[RIGHT].at(0)) );
    REQUIRE( skeleton.valid(toes[RIGHT].at(1)) );
    REQUIRE( skeleton.valid(toes[RIGHT].at(2)) );
    REQUIRE( skeleton.valid(toes[RIGHT].at(3)) );
    REQUIRE( skeleton.valid(toes[RIGHT].at(4)) );

    REQUIRE( skeleton.valid(collerball[LEFT]) );
    REQUIRE( skeleton.valid(arm2[LEFT]) );
    REQUIRE( skeleton.valid(arm1[LEFT]) );
    REQUIRE( skeleton.valid(hand[LEFT]) );
    REQUIRE( skeleton.valid(fingers[LEFT].at(0)) );
    REQUIRE( skeleton.valid(fingers[LEFT].at(1)) );
    REQUIRE( skeleton.valid(fingers[LEFT].at(2)) );
    REQUIRE( skeleton.valid(fingers[LEFT].at(3)) );
    REQUIRE( skeleton.valid(fingers[LEFT].at(4)) );
    REQUIRE( skeleton.valid(femur[LEFT]) );
    REQUIRE( skeleton.valid(lower_leg[LEFT]) );
    REQUIRE( skeleton.valid(foot[LEFT]) );
    REQUIRE( skeleton.valid(toes[LEFT].at(0)) );
    REQUIRE( skeleton.valid(toes[LEFT].at(1)) );
    REQUIRE( skeleton.valid(toes[LEFT].at(2)) );
    REQUIRE( skeleton.valid(toes[LEFT].at(3)) );
    REQUIRE( skeleton.valid(toes[LEFT].at(4)) );
}
