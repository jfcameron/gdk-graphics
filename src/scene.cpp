// © Joseph Cameron - All Rights Reserved

#include <gdk/scene.h>
#include <gdk/entity_owner.h>

#include <memory>

using namespace gdk;

void scene::add(std::shared_ptr<entity_owner> pOwner)
{
    auto pEntities = pOwner->get_entities();

    for (auto pEntity : pEntities) add(pEntity);
}

void scene::remove(std::shared_ptr<entity_owner> pOwner)
{
    auto pEntities = pOwner->get_entities();

    for (auto pEntity : pEntities) remove(pEntity);
}

