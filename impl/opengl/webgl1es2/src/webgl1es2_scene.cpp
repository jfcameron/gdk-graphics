#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

using namespace gdk;

void webgl1es2_scene::add_camera(camera_ptr_type pCamera)
{
    m_cameras.insert(pCamera);
}

bool webgl1es2_scene::contains_camera(camera_ptr_type pCamera) const
{
    auto search = m_cameras.find(pCamera);

    return search != m_cameras.end();
}

void webgl1es2_scene::remove_camera(camera_ptr_type pCamera)
{
    auto search = m_cameras.find(pCamera);
    
    if (search != m_cameras.end())
    {
        m_cameras.erase(search);
    }
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
}

void webgl1es2_scene::draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    for (auto &current_camera : m_cameras)
    {
        static_cast<webgl1es2_camera *>(current_camera.get())->activate(aFrameBufferSize);

        for (auto &[current_material, current_model_to_entity_collection] : m_MaterialToModelToEntityCollection)
        {
            current_material->activate(); 

            for (auto &[current_model, current_entity_collection] : current_model_to_entity_collection)
            {
                current_model->bind(*current_material->getShaderProgram());

                for (auto &current_entity : current_entity_collection) 
                {
                    auto current_entity_impl = static_cast<webgl1es2_entity *>(current_entity.get());
                    
                    current_entity_impl->draw(
                        current_camera->getViewMatrix(), 
                        current_camera->getProjectionMatrix());
                }
            }
        }

        //for (transparent_materals....) //TODO handle blended materials.
    }
}

