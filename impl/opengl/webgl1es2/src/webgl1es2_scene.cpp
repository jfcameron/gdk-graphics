// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_scene.h>

#include <gdk/math_ops.h>

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

void scene_uniforms::apply(const webgl1es2_shader_program &aProgram) const {
    const auto upload = [&aProgram](const auto &aCollection) {
        for (const auto &[name, value] : aCollection)
            if (const auto location = aProgram.uniform_location(name); location != -1)
                aProgram.set_uniform(location, value);
    };

    upload(floats);
    upload(vector2s);
    upload(vector3s);
    upload(vector4s);
    upload(integers);
}

namespace {
    void bind(webgl1es2_material &aMaterial, gl_state &aState, const scene_uniforms &aUniforms) {
        const auto pProgram = aMaterial.getShaderProgram();

        pProgram->useProgram(aState);

        aUniforms.apply(*pProgram);

        aMaterial.activate(aState);
    }
}

void sorted_render_set::draw(const webgl1es2_camera *pCamera, gl_state &aState,
    const frustum &aFrustum, const matrix4x4_type &aViewProjection,
    const scene_uniforms &aUniforms) const {
    prune();

    std::vector<std::shared_ptr<const entity>> live;

    live.reserve(m_unique_entities.size());

    for (const auto &[address, weak] : m_unique_entities)
        if (auto pEntity = weak.lock()) live.push_back(std::move(pEntity));

    const auto cameraPosition = pCamera->get_world_matrix().translation();

    const bool alongView = pCamera->is_orthographic();

    const auto &view = pCamera->get_view_matrix();

    std::vector<std::pair<floating_point_type, const webgl1es2_entity *>> byDepth;

    byDepth.reserve(live.size());

    for (const auto &pLocked : live) {
        const auto *const pEntity = static_cast<const webgl1es2_entity *>(pLocked.get());

        const auto at = pEntity->getModelMatrix().translation();

        // the view looks down -z, so what is farther along it is more negative
        byDepth.emplace_back(alongView ? -(view * at).z : (at - cameraPosition).length_squared(), pEntity);
    }

    std::sort(byDepth.begin(), byDepth.end(),
        [](const auto &aA, const auto &aB) { return aA.first > aB.first; });

    for (const auto &[depth, pEntity] : byDepth) {
        if (culled(*pEntity, aFrustum)) continue;

        const auto &pMaterial = pEntity->getMaterial();

        bind(*pMaterial, aState, aUniforms);

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
    const frustum &aFrustum, const matrix4x4_type &aViewProjection,
    const scene_uniforms &aUniforms) const {
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
                    bind(*current_material, aState, aUniforms);

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

        m_opaque_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum, viewProjection,
            m_Uniforms);

        m_translucent_set.draw(current_texture_camera.get(), *m_pState, cameraFrustum,
            viewProjection, m_Uniforms);
    }

    for (auto &current_screen_camera : live(m_screen_cameras)) {
        current_screen_camera->activate(aFrameBufferSize);

        const auto viewProjection = current_screen_camera->get_projection_matrix()
            * current_screen_camera->get_view_matrix();

        const frustum cameraFrustum(viewProjection);

        m_opaque_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum, viewProjection,
            m_Uniforms);

        m_translucent_set.draw(current_screen_camera.get(), *m_pState, cameraFrustum,
            viewProjection, m_Uniforms);
    }
}

void webgl1es2_scene::set_float(const std::string_view aName, float aValue) {
    m_Uniforms.floats[std::string(aName)] = aValue;
}

void webgl1es2_scene::set_vector2(const std::string_view aName, vector2_type aValue) {
    m_Uniforms.vector2s[std::string(aName)] = aValue;
}

void webgl1es2_scene::set_vector3(const std::string_view aName, vector3_type aValue) {
    m_Uniforms.vector3s[std::string(aName)] = aValue;
}

void webgl1es2_scene::set_vector4(const std::string_view aName, vector4_type aValue) {
    m_Uniforms.vector4s[std::string(aName)] = aValue;
}

void webgl1es2_scene::set_vector4(const std::string_view aName, const color &aValue) {
    set_vector4(aName, vector4_type(aValue.r, aValue.g, aValue.b, aValue.a));
}

void webgl1es2_scene::set_integer(const std::string_view aName, int aValue) {
    m_Uniforms.integers[std::string(aName)] = aValue;
}

webgl1es2_scene::webgl1es2_scene(std::shared_ptr<gl_state> apState)
: m_pState(std::move(apState))
{}
