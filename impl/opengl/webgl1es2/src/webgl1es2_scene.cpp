#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

using namespace gdk;

void webgl1es2_scene::add_camera(camera_ptr_type pCamera)
{
    m_cameras.insert(pCamera);
}

void webgl1es2_scene::add_entity(entity_ptr_type pEntity)
{
    //todo... find the place
}

void webgl1es2_scene::draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    for (auto &current_camera : m_cameras)
    {
        current_camera->activate(aFrameBufferSize); //set up screen & framebuff state

        for (auto &[current_material, current_model_to_entity_collection] : m_MaterialToModelToEntityCollectionCollection)
        {
           //sets up the current shader, much of the pipeline state and material-wide uniform values
           current_material->activate(); 

           for (auto &[current_model, current_entity_collection] : current_model_to_entity_collection)
           {
               current_model->bind(*current_material->m_pShader); //sets up the current vertex data

               for (auto &current_entity : current_entity_collection) 
               {
                   current_entity->draw(current_camera->m_ViewMatrix, current_camera->m_ProjectionMatrix);
               }
           }
        }

        //for (transparent_materals....) //TODO handle blended materials.
    }
}

