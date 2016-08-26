#include "composition_test.h"
#include <catch.hpp>


using namespace ophidian::entity_system;

PartEntity::PartEntity(uint32_t id, EntitySystemBase *system) :
    EntityBase(id, system)
{

}

PartEntity::PartEntity() :
    EntityBase()
{

}

WholeEntity::WholeEntity(uint32_t id, EntitySystemBase *system) :
    EntityBase(id, system)
{

}

WholeEntity::WholeEntity() :
    EntityBase()
{

}

namespace fixture
{

PartAndWholeSystem::PartAndWholeSystem() :
    composition_(whole_, part_)
{

}

WholeSystem &PartAndWholeSystem::wholes()
{
    return whole_;
}

PartSystem &PartAndWholeSystem::parts()
{
    return part_;
}

SimpleComposition &PartAndWholeSystem::composition()
{
    return composition_;
}

}

using namespace fixture;

TEST_CASE_METHOD(PartAndWholeSystem, "Composition: empty", "[entity_system][Property][Composition][EntitySystem]")
{
    REQUIRE( parts().empty() );
    REQUIRE( wholes().empty() );
    REQUIRE( composition().empty() );
}

TEST_CASE_METHOD(AddWhole<PartAndWholeSystem>, "Composition: whole without parts", "[entity_system][Property][Composition][EntitySystem]")
{
    REQUIRE( composition().parts(whole()).empty() );
}

TEST_CASE_METHOD(AddPart<PartAndWholeSystem>, "Composition: unassigned part", "[entity_system][Property][Composition][EntitySystem]")
{
    REQUIRE( composition().whole(part()) == WholeSystem::Entity() );
}

TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: add part to whole", "[entity_system][Property][Composition][EntitySystem]")
{   
    auto const & partsOfWhole = composition().parts(whole());
    REQUIRE( !partsOfWhole.empty() );
    REQUIRE( std::count(partsOfWhole.begin(), partsOfWhole.end(), part()) == 1 );
    REQUIRE( composition().whole(part()) == whole() );
}

TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: remove part from whole", "[entity_system][Property][Composition][EntitySystem]")
{
    composition().erasePart(whole(), part());
    REQUIRE( composition().parts(whole()).empty() );
    REQUIRE( composition().whole(part()) == WholeSystem::Entity() );
}


TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: erase whole, erase part", "[entity_system][Property][Composition][EntitySystem]")
{
    wholes().erase(whole());
    REQUIRE( !parts().valid(part()) );
}

TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: erase part, remove from whole", "[entity_system][Property][Composition][EntitySystem]")
{
    parts().erase(part());
    REQUIRE( composition().parts(whole()).empty() );
}

TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: clear parts, remove from whole", "[entity_system][Property][Composition][EntitySystem]")
{
    parts().clear();
    REQUIRE( composition().parts(whole()).empty() );
}


TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "Composition: clear wholes, erase parts", "[entity_system][Property][Composition][EntitySystem]")
{
    wholes().clear();
    REQUIRE( parts().empty() );
}

TEST_CASE_METHOD(AddPartToWhole< AddWhole<AddPart<PartAndWholeSystem> > >, "[entity_system][Property][Composition][EntitySystem]")
{
    auto nonAttached = parts().add();
    wholes().clear();
    REQUIRE( !parts().valid(part()) );
    REQUIRE( parts().valid(nonAttached) );
}


