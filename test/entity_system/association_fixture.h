#ifndef ASSOCIATION_FIXTURE_H
#define ASSOCIATION_FIXTURE_H

#include "entity_system_fixture.h"
#include <ophidian/entity_system/association.h>

namespace association_fixture {

using namespace ophidian::entity_system;

template <class AssociationType>
class whole_composed_of_three_parts_fixture_plus_one_unattached_part {
    entity_system_test::system_with_1_entity m_wholes;
    entity_system_test::system_with_4_entities m_parts;
    AssociationType m_relation;
public:
    whole_composed_of_three_parts_fixture_plus_one_unattached_part() : m_relation(m_wholes.sys, m_parts.sys) {
        attach_component(m_relation, whole(), m_parts.en.begin(), m_parts.en.end());
        m_relation.dettach_component(whole(), m_parts.en.back()); // last part is unassigned
    }

    entity_system::entity whole() const {
        return m_wholes.entity<0>();
    }

    template <int N>
    entity_system::entity part() const {
        return m_parts.entity<N>();
    }

    entity_system & composite() {
        return m_wholes.sys;
    }

    entity_system & component() {
        return m_parts.sys;
    }


    AssociationType& relation() {
        return m_relation;
    }
};

}

#endif // ASSOCIATION_FIXTURE_H
