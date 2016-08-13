#include <catch.hpp>

#include <ophidian/entity_system/aggregation.h>

#include "association_fixture.h"

using namespace ophidian::entity_system;

using whole_composed_of_three_parts_fixture = association_fixture::whole_composed_of_three_parts_fixture_plus_one_unattached_part<aggregation>;

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "aggregation: 'whole_composed_of_three_parts_fixture' test", "[aggregation]") {
    REQUIRE( composite().size() == 1 );
    REQUIRE( component().size() == 4 );
    REQUIRE( relation().components_size(whole()) == 3 );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "aggregation: destroy whole, dettach parts", "[aggregation]") {
    composite().destroy(whole());
    REQUIRE( component().valid(part<0>()) );
    REQUIRE( component().valid(part<1>()) );
    REQUIRE( component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE( relation().composite_of(part<0>()) == entity_system::entity::null() );
    REQUIRE( relation().composite_of(part<1>()) == entity_system::entity::null() );
    REQUIRE( relation().composite_of(part<2>()) == entity_system::entity::null() );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "aggregation: clear whole system, dettach parts", "[aggregation]") {
    composite().clear();
    REQUIRE( component().valid(part<0>()) );
    REQUIRE( component().valid(part<1>()) );
    REQUIRE( component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE( relation().composite_of(part<0>()) == entity_system::entity::null() );
    REQUIRE( relation().composite_of(part<1>()) == entity_system::entity::null() );
    REQUIRE( relation().composite_of(part<2>()) == entity_system::entity::null() );
}


TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "aggregation: destroy part, remove from whole", "[aggregation]") {
    component().destroy(part<0>());
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE( relation().is_component_of(whole(), part<1>()) );
    REQUIRE( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 2 );
    REQUIRE( relation().composite_of(part<1>()) == whole() );
    REQUIRE( relation().composite_of(part<2>()) == whole() );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "aggregation: clear parts, remove from wholes", "[aggregation]") {
    component().clear();
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<1>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 0 );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}


struct self_aggregation_with_root_and_child_fixture {
    entity_system sys;
    aggregation compo{sys, sys};
    entity_system::entity root;
    entity_system::entity child;

    self_aggregation_with_root_and_child_fixture() :
        root(sys.create()),
        child(sys.create())
    {
        compo.attach_component(root, child);
    }
};


TEST_CASE_METHOD(self_aggregation_with_root_and_child_fixture, "aggregation:  'self_aggregation_with_root_and_child_fixture' test", "[aggregation]") {
    REQUIRE( compo.components_size(root) == 1 );
    REQUIRE( compo.composite_of(child) == root );
    REQUIRE( std::find(compo.components_bounds(root).begin(), compo.components_bounds(root).end(), child) != compo.components_bounds(root).end() );
}


TEST_CASE_METHOD(self_aggregation_with_root_and_child_fixture, "aggregation: self aggregation, attach child, destroy parent", "[aggregation]") {
    sys.destroy(root);
    REQUIRE( sys.valid(child) );
    REQUIRE_THROWS( compo.components_size(root) );
    REQUIRE( compo.composite_of(child) == entity_system::entity::null() );
}

TEST_CASE_METHOD(self_aggregation_with_root_and_child_fixture, "aggregation: self aggregation, attach child, destroy child", "[aggregation]") {
    sys.destroy(child);
    REQUIRE( compo.components_size(root) == 0 );
    REQUIRE_THROWS( compo.components_size(child) );
}


