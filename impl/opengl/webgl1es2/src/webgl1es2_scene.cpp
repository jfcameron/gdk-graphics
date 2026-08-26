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


void render_set::prune() const {
    for (auto entity = m_unique_entities.begin(); entity != m_unique_entities.end();)
        entity = entity->second.expired() ? m_unique_entities.erase(entity) : std::next(entity);

    for (auto material = m_MaterialToModelToEntityCollection.begin();
        material != m_MaterialToModelToEntityCollection.end();) {
        auto &models = material->second;

        for (auto model = models.begin(); model != models.end();) {
            auto &entities = model->second;

            entities.erase(std::remove_if(entities.begin(), entities.end(),
                [](const entity_weak_ptr_type &a) { return a.expired(); }), entities.end());

            model = entities.empty() ? models.erase(model) : std::next(model);
        }

        material = models.empty()
            ? m_MaterialToModelToEntityCollection.erase(material)
            : std::next(material);
    }
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

    m_screen_cameras.erase(std::remove_if(m_screen_cameras.begin(), m_screen_cameras.end(),
        [pTarget](const std::weak_ptr<const webgl1es2_screen_camera> &a) {
            const auto pLocked = a.lock();

            return !pLocked || pLocked.get() == pTarget;
        }), m_screen_cameras.end());
}

void webgl1es2_scene::remove(const std::shared_ptr<const texture_camera> &pCamera) {
    if (!pCamera.get()) return;

    const auto *const pTarget = pCamera.get();

    m_texture_cameras.erase(std::remove_if(m_texture_cameras.begin(), m_texture_cameras.end(),
        [pTarget](const std::weak_ptr<const webgl1es2_texture_camera> &a) {
            const auto pLocked = a.lock();

            return !pLocked || pLocked.get() == pTarget;
        }), m_texture_cameras.end());
}

void sorted_render_set::draw(const webgl1es2_camera *pCamera, gl_state &aState,
    const frustum &aFrustum) const {
    prune();

    std::vector<std::shared_ptr<const entity>> sorted_entities;

    sorted_entities.reserve(m_unique_entities.size());

    for (const auto &[address, weak] : m_unique_entities)
        if (auto pEntity = weak.lock()) sorted_entities.push_back(std::move(pEntity));

    std::sort(sorted_entities.begin(), sorted_entities.end(),
    [pCamera](std::shared_ptr<const entity> pA, std::shared_ptr<const entity> pB) {
        const auto cameraPos = static_cast<const webgl1es2_camera *>(pCamera)->get_world_matrix().translation();
        const auto entityPosA = static_cast<const webgl1es2_entity *>(pA.get())->getModelMatrix().translation();
        const auto entityPosB = static_cast<const webgl1es2_entity *>(pB.get())->getModelMatrix().translation();

        const auto aDist = cameraPos.distance_from(entityPosA);
        const auto bDist = cameraPos.distance_from(entityPosB);

        return (aDist > bDist);
    });

    for (auto current_entity : sorted_entities) {
        auto pEntity = static_cast<const webgl1es2_entity *>(current_entity.get());

        if (culled(*pEntity, aFrustum)) continue;

        auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());
        pMaterial->activate(aState);

        auto pModel = std::static_pointer_cast<webgl1es2_model>(pEntity->getModel());
        pModel->bind(*pMaterial->getShaderProgram());

        pEntity->draw(pCamera->get_view_matrix(),
            pCamera->get_projection_matrix());
    }
}

void sorted_render_set::try_add(entity_ptr_type pEntityInterface) {
    prune();

    m_unique_entities.emplace(pEntityInterface.get(), pEntityInterface);
}

void render_set::try_add(entity_ptr_type pEntityInterface) {
    prune();

    if (!m_unique_entities.emplace(pEntityInterface.get(), pEntityInterface).second) return;

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());
    auto pModel = std::static_pointer_cast<webgl1es2_model>(pEntity->getModel());
    auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

    m_MaterialToModelToEntityCollection[pMaterial][pModel].push_back(pEntityInterface);
}

void webgl1es2_scene::add(const std::shared_ptr<const entity> &pEntityInterface) {
    if (!pEntityInterface.get()) return;

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());
    auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

    switch(pMaterial->get_render_mode()) {
        case material::render_mode::opaque: m_opaque_set.try_add(pEntityInterface); return;
        case material::render_mode::transparent: m_translucent_set.try_add(pEntityInterface); return;
        default: break;
    }
    throw exception("unhandled material render mode");
}

void render_set::draw(const webgl1es2_camera *pCamera, gl_state &aState,
    const frustum &aFrustum) const {
    prune();

    for (auto &[current_material, current_model_to_entity_collection] :
        m_MaterialToModelToEntityCollection) {
        current_material->activate(aState);

        for (auto &[current_model, current_entity_collection]
            : current_model_to_entity_collection) {
            current_model->bind(*current_material->getShaderProgram());

            for (auto &current_entity : current_entity_collection) {
                const auto pLocked = current_entity.lock();

                if (!pLocked) continue;

                const auto *const pEntity = static_cast<const webgl1es2_entity *>(pLocked.get());

                if (culled(*pEntity, aFrustum)) continue;

                pEntity->draw(pCamera->get_view_matrix(), pCamera->get_projection_matrix());
            }
        }
    }
}

void webgl1es2_scene::draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const {
    const auto live = [](auto &aCameras) {
        using camera_type = typename std::decay_t<decltype(aCameras)>::value_type::element_type;

        std::vector<std::shared_ptr<const camera_type>> out;

        aCameras.erase(std::remove_if(aCameras.begin(), aCameras.end(),
            [&out](const std::weak_ptr<const camera_type> &a) {
                if (auto pLocked = a.lock()) {
                    out.push_back(std::move(pLocked));

                    return false;
                }

                return true;
            }), aCameras.end());

        return out;
    };

    for (auto &current_texture_camera : live(m_texture_cameras)) {
        current_texture_camera->activate();

        const frustum cameraFrustum(current_texture_camera->get_projection_matrix()
            * current_texture_camera->get_view_matrix());

        m_opaque_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum);

        m_translucent_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum);
    }

    for (auto &current_screen_camera : live(m_screen_cameras)) {
        current_screen_camera->activate(aFrameBufferSize);

        const frustum cameraFrustum(current_screen_camera->get_projection_matrix()
            * current_screen_camera->get_view_matrix());

        m_opaque_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum);

        m_translucent_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum);
    }
}

webgl1es2_scene::webgl1es2_scene(std::shared_ptr<gl_state> apState)
: m_pState(std::move(apState))
{}
