// © Joseph Cameron - All Rights Reserved

#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

#include <algorithm>
#include <list>
#include <vector>

using namespace gdk;

void webgl1es2_scene::remove(const std::shared_ptr<const entity> &pEntity)
{
    //TODO: implement
}

void webgl1es2_scene::add(const std::shared_ptr<const screen_camera> &pCamera)
{
    if (!pCamera.get()) return;

    m_screen_cameras.insert(
        std::static_pointer_cast<const webgl1es2_screen_camera>(pCamera));
}

void webgl1es2_scene::add(const std::shared_ptr<const texture_camera> &pCamera)
{
    if (!pCamera.get()) return;
    
    m_texture_cameras.insert(
        std::static_pointer_cast<const webgl1es2_texture_camera>(pCamera));
}

void webgl1es2_scene::remove(const std::shared_ptr<const screen_camera> &pCamera)
{
    if (!pCamera.get()) return;

    auto search = m_screen_cameras.find(
        std::static_pointer_cast<const webgl1es2_screen_camera>(pCamera));
    
    if (search != m_screen_cameras.end()) m_screen_cameras.erase(search);
}

void webgl1es2_scene::remove(const std::shared_ptr<const texture_camera> &pCamera)
{
    if (!pCamera.get()) return;

    auto search = m_texture_cameras.find(
        std::static_pointer_cast<const webgl1es2_texture_camera>(pCamera));
    
    if (search != m_texture_cameras.end()) m_texture_cameras.erase(search);
}

void sorted_render_set::draw(const webgl1es2_camera *pCamera) const
{
    std::vector<std::shared_ptr<const entity>> sorted_entities(
        m_unique_entities.begin(), 
        m_unique_entities.end());

    std::sort(sorted_entities.begin(), sorted_entities.end(),
    [pCamera](std::shared_ptr<const entity> pA, std::shared_ptr<const entity> pB)
    {
        const auto cameraPos = static_cast<const webgl1es2_camera *>(pCamera)->get_view_matrix().translation();
        const auto entityPosA = static_cast<const webgl1es2_entity *>(pA.get())->getModelMatrix().translation();
        const auto entityPosB = static_cast<const webgl1es2_entity *>(pB.get())->getModelMatrix().translation();

        const auto aDist = cameraPos.distance(entityPosA);
        const auto bDist = cameraPos.distance(entityPosB);

        return (aDist > bDist);
    });

    for (auto current_entity : sorted_entities)
    {
        auto pEntity = static_cast<const webgl1es2_entity *>(current_entity.get());
        auto pModel = std::static_pointer_cast<webgl1es2_model>(pEntity->getModel());
        auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

        const auto entityPos = static_cast<const webgl1es2_entity *>(pEntity)->getModelMatrix().translation();
        const auto cameraPos = static_cast<const webgl1es2_camera *>(pCamera)->get_view_matrix().translation();
        
        pMaterial->activate();

        pModel->bind(*pMaterial->getShaderProgram());

        pEntity->draw(pCamera->get_view_matrix(), 
            pCamera->get_projection_matrix());
    }
}

void sorted_render_set::try_add(entity_ptr_type pEntityInterface)
{
    if (auto search = m_unique_entities.find(pEntityInterface); 
        search != m_unique_entities.end()) return;

    m_unique_entities.insert(pEntityInterface);
}

void render_set::try_add(entity_ptr_type pEntityInterface)
{
    if (auto search = m_unique_entities.find(pEntityInterface); 
        search != m_unique_entities.end()) return;

    m_unique_entities.insert(pEntityInterface);

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());
    auto pModel = std::static_pointer_cast<webgl1es2_model>(pEntity->getModel());
    auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

    auto modelToEntityCollectionSearch = m_MaterialToModelToEntityCollection.find(pMaterial);

    if (modelToEntityCollectionSearch == m_MaterialToModelToEntityCollection.end())
    {
        m_MaterialToModelToEntityCollection[pMaterial] = {};
        
        m_MaterialToModelToEntityCollection[pMaterial][pModel] = {};
    }
    else if (auto entityCollectionSearch = modelToEntityCollectionSearch->second.find(pModel); 
        entityCollectionSearch == modelToEntityCollectionSearch->second.end())
    {
        modelToEntityCollectionSearch->second[pModel] = {};
    }
    
    m_MaterialToModelToEntityCollection[pMaterial][pModel].insert(pEntityInterface);
}

void webgl1es2_scene::add(const std::shared_ptr<const entity> &pEntityInterface)
{
    if (!pEntityInterface.get()) return;

    auto pEntity = static_cast<const webgl1es2_entity *>(pEntityInterface.get());
    auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

    switch(pMaterial->get_render_mode())
    {
        case material::render_mode::opaque: 
            m_opaque_set.try_add(pEntityInterface); break;

        case material::render_mode::transparent: 
            m_translucent_set.try_add(pEntityInterface); break;
    }
}

void render_set::draw(const webgl1es2_camera *pCamera) const
{
    for (auto &[current_material, current_model_to_entity_collection] : 
        m_MaterialToModelToEntityCollection)
    {
        current_material->activate(); 

        for (auto &[current_model, current_entity_collection] 
            : current_model_to_entity_collection)
        {
            current_model->bind(*current_material->getShaderProgram());

            for (auto &current_entity : current_entity_collection) 
            {
                static_cast<const webgl1es2_entity *>(current_entity.get())->draw(
                    pCamera->get_view_matrix(), 
                    pCamera->get_projection_matrix());
            }
        }
    }
}

void webgl1es2_scene::draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    for (auto &current_texture_camera : m_texture_cameras)
    {
        static_cast<const webgl1es2_texture_camera *>(current_texture_camera.get())->activate();

        m_opaque_set.draw(current_texture_camera.get());

        m_translucent_set.draw(current_texture_camera.get());
    }
    
    for (auto &current_screen_camera : m_screen_cameras)
    {
        static_cast<const webgl1es2_screen_camera *>(current_screen_camera.get())->activate(aFrameBufferSize);

        m_opaque_set.draw(current_screen_camera.get());

        m_translucent_set.draw(current_screen_camera.get());
    }
}

