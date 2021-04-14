#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

using namespace gdk;

void webgl1es2_scene::add_camera(std::shared_ptr<screen_camera> pCamera)
{
    m_screen_cameras.insert(
        std::static_pointer_cast<webgl1es2_screen_camera>(pCamera));
}

void webgl1es2_scene::add_camera(std::shared_ptr<texture_camera> pCamera)
{
    m_texture_cameras.insert(
        std::static_pointer_cast<webgl1es2_texture_camera>(pCamera));
}

bool webgl1es2_scene::contains_camera(std::shared_ptr<screen_camera> pCamera) const
{
    auto search = m_screen_cameras.find(std::static_pointer_cast<webgl1es2_screen_camera>(pCamera));

    return search != m_screen_cameras.end();
}

bool webgl1es2_scene::contains_camera(std::shared_ptr<texture_camera> pCamera) const
{
    auto search = m_texture_cameras.find(std::static_pointer_cast<webgl1es2_texture_camera>(pCamera));

    return search != m_texture_cameras.end();
}

void webgl1es2_scene::remove_camera(std::shared_ptr<screen_camera> pCamera)
{
    auto search = m_screen_cameras.find(std::static_pointer_cast<webgl1es2_screen_camera>(pCamera));
    
    if (search != m_screen_cameras.end()) m_screen_cameras.erase(search);
}

void webgl1es2_scene::remove_camera(std::shared_ptr<texture_camera> pCamera)
{
    auto search = m_texture_cameras.find(std::static_pointer_cast<webgl1es2_texture_camera>(pCamera));
    
    if (search != m_texture_cameras.end()) m_texture_cameras.erase(search);
}

void sorted_render_set::sort()
{
    //TODO: implement
}

void sorted_render_set::try_add_entity(entity_ptr_type pEntityInterface)
{
    /*if (auto search = m_unique_entities.find(pEntityInterface); 
        search != m_unique_entities.end()) return;

    m_unique_entities.insert(pEntityInterface);

    auto pEntity = static_cast<webgl1es2_entity *>(pEntityInterface.get());
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
    
    m_MaterialToModelToEntityCollection[pMaterial][pModel].insert(pEntityInterface);*/
}

void render_set::try_add_entity(entity_ptr_type pEntityInterface)
{
    /*if (auto search = m_unique_entities.find(pEntityInterface); 
        search != m_unique_entities.end()) return;*/

    m_unique_entities.insert(pEntityInterface);

    auto pEntity = static_cast<webgl1es2_entity *>(pEntityInterface.get());
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

void webgl1es2_scene::add_entity(entity_ptr_type pEntityInterface)
{
    auto pEntity = static_cast<webgl1es2_entity *>(pEntityInterface.get());
    auto pMaterial = std::static_pointer_cast<webgl1es2_material>(pEntity->getMaterial());

    switch(pMaterial->get_render_mode())
    {
        case material::render_mode::opaque: 
            m_opaque_set.try_add_entity(pEntityInterface); break;

        case material::render_mode::transparent: 
            m_translucent_set.try_add_entity(pEntityInterface); break;
    }
}

void webgl1es2_scene::remove_entity(entity_ptr_type pEntity)
{
    //TODO: implement
}

void render_set::draw(webgl1es2_camera *pCamera) const
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
                static_cast<webgl1es2_entity *>(current_entity.get())->draw(
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
        static_cast<webgl1es2_texture_camera *>(current_texture_camera.get())->activate();

        m_opaque_set.draw(current_texture_camera.get());

        m_translucent_set.draw(current_texture_camera.get());
    }
    
    for (auto &current_screen_camera : m_screen_cameras)
    {
        static_cast<webgl1es2_screen_camera *>(current_screen_camera.get())->activate(aFrameBufferSize);

        m_opaque_set.draw(current_screen_camera.get());

        m_translucent_set.draw(current_screen_camera.get());
    }
}

