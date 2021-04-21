// © Joseph Cameron - All Rights Reserved

#include <gdk/scene.h>
#include <gdk/entity_owner.h>

#include <memory>

using namespace gdk;

void scene::add(const entity_owner &rOwner)
{
    const auto pEntities = rOwner.get_entities();

    for (auto pEntity : pEntities) add(pEntity);
}

void scene::remove(const entity_owner &rOwner)
{
    const auto pEntities = rOwner.get_entities();

    for (const auto &pEntity : pEntities) remove(pEntity);
}

