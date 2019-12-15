#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

using namespace gdk;

void webgl1es2_scene::add_camera(camera_ptr_type pCamera)
{
    m_cameras.insert(pCamera);
}

void webgl1es2_scene::add_entity(entity_ptr_type pEntityInterface)
{
    //todo... find the place
    auto pEntity = static_cast<webgl1es2_entity *>(pEntityInterface.get());

    auto pModel = pEntity->m_model;
    auto pMaterial = pEntity->m_Material;

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

void webgl1es2_scene::draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    for (auto &current_camera : m_cameras)
    {
        static_cast<webgl1es2_camera *>(current_camera.get())->activate(aFrameBufferSize); //set up screen & framebuff state

        for (auto &[current_material, current_model_to_entity_collection] : m_MaterialToModelToEntityCollection)
        {
            //sets up the current shader, much of the pipeline state and material-wide uniform values
            current_material->activate(); 

            for (auto &[current_model, current_entity_collection] : current_model_to_entity_collection)
            {
                current_model->bind(*current_material->m_pShaderProgram); //sets up the current vertex data

                for (auto &current_entity : current_entity_collection) 
                {
                    static_cast<webgl1es2_entity *>(current_entity.get())->draw(
                        current_camera->getViewMatrix(), 
                        current_camera->getProjectionMatrix());
                }
            }
        }

        //for (transparent_materals....) //TODO handle blended materials.
    }
}

