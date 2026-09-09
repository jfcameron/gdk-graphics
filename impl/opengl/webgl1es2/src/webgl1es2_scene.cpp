// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_scene.h>

#include <algorithm>
#include <list>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    [[nodiscard]] bool culled(const webgl1es2_entity &aEntity, const frustum &aFrustum) {
        if (aEntity.is_hidden()) return true;

        vector3_type centre;
        floating_point_type radius;

        aEntity.world_bounds(centre, radius);

        return aFrustum.excludes(centre, radius);
    }
}


bool render_set::claim(const entity_ptr_type &apEntity) const {
    const auto [entry, inserted] = m_unique_entities.try_emplace(apEntity.get(), apEntity);

    if (inserted) return true;

    if (!entry->second.expired()) return false;

    entry->second = apEntity;

    return true;
}

void render_set::prune() const {
    if (++m_SinceLastPrune < PRUNE_INTERVAL) return;

    m_SinceLastPrune = 0;

    const auto dead = [](const auto &aPair) { return aPair.second.expired(); };

    std::erase_if(m_unique_entities, dead);

    std::erase_if(m_MaterialToModelToEntityCollection, [](auto &aMaterial) {
        std::erase_if(aMaterial.second, [](auto &aModel) {
            std::erase_if(aModel.second,
                [](const entity_weak_ptr_type &a) { return a.expired(); });

            return aModel.second.empty();
        });

        return aMaterial.second.empty();
    });
}

std::size_t render_set::entity_count() const {
    prune();

    return m_unique_entities.size();
}

void render_set::remove(const entity *const apEntity) {
    if (!apEntity) return;

    m_unique_entities.erase(apEntity);

    for (auto &[material, models] : m_MaterialToModelToEntityCollection)
        for (auto &[model, entities] : models)
            entities.erase(std::remove_if(entities.begin(), entities.end(),
                [apEntity](const entity_weak_ptr_type &a) {
                    const auto pLocked = a.lock();

                    return !pLocked || pLocked.get() == apEntity;
                }), entities.end());

    prune();
}

void webgl1es2_scene::remove(const std::shared_ptr<const entity> &pEntity) {
    if (!pEntity) return;

    m_opaque_set.remove(pEntity.get());
    m_translucent_set.remove(pEntity.get());
}

void webgl1es2_scene::add(const std::shared_ptr<const screen_camera> &pCamera) {
    if (!pCamera.get()) return;

    m_screen_cameras.push_back(
        std::static_pointer_cast<const webgl1es2_screen_camera>(pCamera));
}

void webgl1es2_scene::add(const std::shared_ptr<const texture_camera> &pCamera) {
    if (!pCamera.get()) return;
    
    m_texture_cameras.push_back(
        std::static_pointer_cast<const webgl1es2_texture_camera>(pCamera));
}

void webgl1es2_scene::remove(const std::shared_ptr<const screen_camera> &pCamera) {
    if (!pCamera.get()) return;

    const auto *const pTarget = pCamera.get();

    std::erase_if(m_screen_cameras, [pTarget](const std::weak_ptr<const webgl1es2_screen_camera> &a) {
        const auto pLocked = a.lock();

        return !pLocked || pLocked.get() == pTarget;
    });
}

void webgl1es2_scene::remove(const std::shared_ptr<const texture_camera> &pCamera) {
    if (!pCamera.get()) return;

    const auto *const pTarget = pCamera.get();

    std::erase_if(m_texture_cameras, [pTarget](const std::weak_ptr<const webgl1es2_texture_camera> &a) {
        const auto pLocked = a.lock();

        return !pLocked || pLocked.get() == pTarget;
    });
}

void sorted_render_set::draw(const webgl1es2_camera *pCamera, gl_state &aState,
    const frustum &aFrustum, const matrix4x4_type &aViewProjection) const {
    prune();

    std::vector<std::shared_ptr<const entity>> live;

    live.reserve(m_unique_entities.size());

    for (const auto &[address, weak] : m_unique_entities)
        if (auto pEntity = weak.lock()) live.push_back(std::move(pEntity));

    const auto cameraPosition = pCamera->get_world_matrix().translation();

    std::vector<std::pair<floating_point_type, const webgl1es2_entity *>> byDepth;

    byDepth.reserve(live.size());

    for (const auto &pLocked : live) {
        const auto *const pEntity = static_cast<const webgl1es2_entity *>(pLocked.get());

        byDepth.emplace_back(
            (pEntity->getModelMatrix().translation() - cameraPosition).length_squared(), pEntity);
    }

    std::sort(byDepth.begin(), byDepth.end(),
        [](const auto &aA, const auto &aB) { return aA.first > aB.first; });

    for (const auto &[depth, pEntity] : byDepth) {
        if (culled(*pEntity, aFrustum)) continue;

        const auto &pMaterial = pEntity->getMaterial();

        pMaterial->activate(aState);

        pEntity->getModel()->bind(*pMaterial->getShaderProgram());

        pEntity->draw(pCamera->get_view_matrix(), pCamera->get_projection_matrix(),
            aViewProjection);
    }
}

void sorted_render_set::try_add(entity_ptr_type pEntityInterface) {
    static_cast<void>(claim(pEntityInterface));
}

void render_set::try_add(entity_ptr_type pEntityInterface) {
    if (!claim(pEntityInterface)) return;

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());

    m_MaterialToModelToEntityCollection[pEntity->getMaterial()][pEntity->getModel()]
        .push_back(pEntityInterface);
}

void webgl1es2_scene::add(const std::shared_ptr<const entity> &pEntityInterface) {
    if (!pEntityInterface.get()) return;

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());

    switch(pEntity->getMaterial()->get_render_mode()) {
        case material::render_mode::opaque: m_opaque_set.try_add(pEntityInterface); return;
        case material::render_mode::transparent: m_translucent_set.try_add(pEntityInterface); return;
        default: break;
    }
    throw exception("unhandled material render mode");
}

void render_set::draw(const webgl1es2_camera *pCamera, gl_state &aState,
    const frustum &aFrustum, const matrix4x4_type &aViewProjection) const {
    prune();

    for (auto &[current_material, current_model_to_entity_collection] :
        m_MaterialToModelToEntityCollection) {
        bool materialActive = false;

        for (auto &[current_model, current_entity_collection]
            : current_model_to_entity_collection) {
            bool modelBound = false;

            for (auto &current_entity : current_entity_collection) {
                const auto pLocked = current_entity.lock();

                if (!pLocked) continue;

                const auto *const pEntity = static_cast<const webgl1es2_entity *>(pLocked.get());

                if (culled(*pEntity, aFrustum)) continue;

                if (!materialActive) {
                    current_material->activate(aState);

                    materialActive = true;
                }

                if (!modelBound) {
                    current_model->bind(*current_material->getShaderProgram());

                    modelBound = true;
                }

                pEntity->draw(pCamera->get_view_matrix(), pCamera->get_projection_matrix(),
                    aViewProjection);
            }
        }
    }
}

void webgl1es2_scene::draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const {
    const auto live = [](auto &aCameras) {
        using camera_type = typename std::decay_t<decltype(aCameras)>::value_type::element_type;

        std::vector<std::shared_ptr<const camera_type>> out;

        std::erase_if(aCameras, [&out](const std::weak_ptr<const camera_type> &a) {
            if (auto pLocked = a.lock()) {
                out.push_back(std::move(pLocked));

                return false;
            }

            return true;
        });

        return out;
    };

    for (auto &current_texture_camera : live(m_texture_cameras)) {
        current_texture_camera->activate();

        const auto viewProjection = current_texture_camera->get_projection_matrix()
            * current_texture_camera->get_view_matrix();

        const frustum cameraFrustum(viewProjection);

        m_opaque_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum, viewProjection);

        m_translucent_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum, viewProjection);
    }

    for (auto &current_screen_camera : live(m_screen_cameras)) {
        current_screen_camera->activate(aFrameBufferSize);

        const auto viewProjection = current_screen_camera->get_projection_matrix()
            * current_screen_camera->get_view_matrix();

        const frustum cameraFrustum(viewProjection);

        m_opaque_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum, viewProjection);

        m_translucent_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum, viewProjection);
    }
}

webgl1es2_scene::webgl1es2_scene(std::shared_ptr<gl_state> apState)
: m_pState(std::move(apState))
{}
