#include <catch.hpp>

#include <ophidian/entity_system/association.h>

using namespace ophidian::entity_system;

class association_mock : public association {
public:
    association_mock(const entity_system & composite, entity_system & component) :
        association(composite, component){
    }

    virtual void on_destroy(entity_system::entity en){
    }

    virtual void on_clear(){
    }

};


struct empty_part_whole_fixture {
    entity_system composite, component;
    association_mock relation;
    empty_part_whole_fixture() :
        relation(composite, component)
    {
    }
};

struct one_part_one_whole_unnasigned_fixture : public empty_part_whole_fixture {
    entity_system::entity whole;
    entity_system::entity part;
    one_part_one_whole_unnasigned_fixture() : whole(composite.create()), part(component.create()) {
    }
};


struct one_part_one_whole_fixture : public one_part_one_whole_unnasigned_fixture {

    one_part_one_whole_fixture() {
        relation.attach_component(whole, part);
    }
};


struct two_parts_one_whole_fixture : public one_part_one_whole_fixture {
    entity_system::entity part2;
    two_parts_one_whole_fixture() : part2(component.create())
    {
        relation.attach_component(whole, part2);
    }
};

struct three_parts_one_whole_fixture : public two_parts_one_whole_fixture {
    entity_system::entity part3;
    three_parts_one_whole_fixture() : part3(component.create()) {
        relation.attach_component(whole, part3);
    }
};

TEST_CASE("association: test fixture","[association]") {
    REQUIRE_NOTHROW( empty_part_whole_fixture() );
}


TEST_CASE("association: constructor & destructor","[association]") {
    entity_system composite;
    entity_system component;

    const int NULL_PROPERTIES_BEFORE = entity_system::null().properties_size();

    association * relation;
    REQUIRE_NOTHROW(relation = new association_mock(composite, component));
    // part-to-whole property, whole-to-part property
    REQUIRE(entity_system::null().properties_size() == NULL_PROPERTIES_BEFORE + 2);
    REQUIRE_NOTHROW(delete relation);

}

TEST_CASE_METHOD(empty_part_whole_fixture, "association: part-whole", "[association]") {
    REQUIRE( relation.composite() == composite );
    REQUIRE( relation.component() == component );
}

TEST_CASE_METHOD(one_part_one_whole_unnasigned_fixture, "association: unattached", "[association]") {
    REQUIRE( !relation.is_component_of(whole, part) );
    REQUIRE( relation.components_size(whole) == 0 );
    REQUIRE( relation.composite_of(part) == entity_system::entity::null() );
    REQUIRE( !relation.is_attached(part) );
}

TEST_CASE_METHOD(one_part_one_whole_unnasigned_fixture, "association: destroy unattached part", "[association]") {
    REQUIRE_NOTHROW( component.destroy(part) );
}

TEST_CASE_METHOD(one_part_one_whole_fixture, "association: attach part to whole", "[association]") {
    REQUIRE( relation.components_size(whole) == 1 );
    REQUIRE( relation.is_component_of(whole, part) );
    REQUIRE( relation.composite_of(part) == whole );
    REQUIRE( relation.is_attached(part) );

}

TEST_CASE_METHOD(one_part_one_whole_fixture, "association: dettach part from whole", "[association]") {
    relation.dettach_component(whole, part);
    REQUIRE( relation.components_size(whole) == 0 );
    REQUIRE( relation.composite_of(part) == entity_system::entity::null() );
    REQUIRE( relation.is_component_of(entity_system::entity::null(), part) );
    REQUIRE_THROWS( relation.dettach_component(whole, part) );
}


TEST_CASE_METHOD(two_parts_one_whole_fixture, "association: get components bounds (two parts)", "[association]") {
    auto bounds = relation.components_bounds(whole);
    REQUIRE( relation.components_size(whole) == 2 );
    REQUIRE( std::distance(bounds.begin(), bounds.end()) == 2 );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part2)  != bounds.end() );
}

TEST_CASE_METHOD(three_parts_one_whole_fixture, "association: get components bounds (three parts)", "[association]") {
    auto bounds = relation.components_bounds(whole);
    REQUIRE( relation.components_size(whole) == 3 );
    REQUIRE( std::distance(bounds.begin(), bounds.end()) == 3 );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part2)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part3)  != bounds.end() );
}

TEST_CASE_METHOD(three_parts_one_whole_fixture, "association: create whole, allocate links to parts", "[association]") {
    auto new_whole = composite.create();
    REQUIRE_NOTHROW( relation.components_size(new_whole) );
    REQUIRE_NOTHROW( relation.components_bounds(new_whole) );
}

TEST_CASE_METHOD(three_parts_one_whole_fixture, "association: create part, allocate link to whole", "[association]") {
    auto new_part = component.create();
    REQUIRE( (relation.composite_of(new_part) == entity_system::entity::null()) );
}


// self association

struct self_association_fixture {
    entity_system sys;
    association_mock assoc{sys, sys};
};

struct self_association_fixture_with_root : public self_association_fixture {
    entity_system::entity root{sys.create()};
};

struct self_association_fixture_with_root_and_child_unattached : public self_association_fixture_with_root {
    entity_system::entity child{sys.create()};
};

struct self_association_fixture_with_root_and_child : public self_association_fixture_with_root_and_child_unattached {
    self_association_fixture_with_root_and_child() {
        assoc.attach_component(root, child);
    }
};

TEST_CASE_METHOD(self_association_fixture_with_root, "compostition: self association", "[association]") {
    REQUIRE( assoc.composite_of(root) == entity_system::entity::null() );
    REQUIRE( assoc.is_component_of(entity_system::entity::null(), root) );
}

TEST_CASE_METHOD(self_association_fixture_with_root_and_child, "compostition: self association, attach child", "[association]") {
    REQUIRE( assoc.components_size(root) == 1 );
    REQUIRE( assoc.composite_of(root) == entity_system::entity::null() );
    REQUIRE( assoc.is_component_of(entity_system::entity::null(), root) );
    REQUIRE( assoc.is_component_of(root, child) );
    REQUIRE( assoc.components_size(child) == 0 );
}

TEST_CASE_METHOD(self_association_fixture_with_root_and_child, "compostition: self association, attach child, dettach child", "[association]") {
    assoc.dettach_component(root, child);
    REQUIRE( assoc.composite_of(child) == entity_system::entity::null() );
    REQUIRE( assoc.is_component_of(entity_system::entity::null(), child));
    REQUIRE( assoc.components_size(root) == 0 );
}
