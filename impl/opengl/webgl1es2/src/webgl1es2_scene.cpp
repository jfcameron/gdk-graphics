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

void webgl1es2_scene::add_entity(entity_ptr_type pEntityInterface)
{
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

void webgl1es2_scene::remove_entity(entity_ptr_type pEntity)
{
    // How naive? iterate all entity sets, remove instances of pEntity.
    //TODO: implement fast? Need a second datastrcture. linear one. a set? A single entity set makes good sense.
    // nested sets replace with sets of size_t? or perhaps iters to the global set. yes. rewrite. indicies.?
    //TODO: could use an unordered_set to track unique entitys. tricky part is thinking of how to link
    // the unordered set entry to the correct location in nested structure so can remove that item at constant time
}

void webgl1es2_scene::draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    //TODO: consider moving this into a "pipeline" or "queue" abstraction. 
    // i think thats what this is becoming
    auto draw_entities = [&](webgl1es2_camera *pCamera)
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
                    auto current_entity_impl = static_cast<webgl1es2_entity *>(
                        current_entity.get());
                    
                    current_entity_impl->draw(
                        pCamera->get_view_matrix(), 
                        pCamera->get_projection_matrix());
                }
            }
        }

        //for (transparent_materals....) //TODO handle blended materials.
    };

    for (auto &current_texture_camera : m_texture_cameras)
    {
        static_cast<webgl1es2_texture_camera *>(current_texture_camera.get())->activate();

        draw_entities(current_texture_camera.get());
    }

    for (auto &current_screen_camera : m_screen_cameras)
    {
        static_cast<webgl1es2_screen_camera *>(current_screen_camera.get())->activate(aFrameBufferSize);

        draw_entities(current_screen_camera.get());
    }
}

