#include <gdk/scene.h>

using namespace gdk;

void scene::add_camera(camera_ptr_type pCamera)
{
    m_cameras.insert(pCamera);
}

void scene::add_entity(entity_ptr_type pEntity)
{
    //todo... find the place
}

void scene::draw()
{
    for (auto &camera : m_cameras)
    {
        /*
           camera.activate(); //set up screen & framebuff state

           for (auto &[material, model_to_entity_collection] : material_to_model_to_entity_collection_collection)
           {
           material->activate(); //sets up the current shader and material-wide uniform values

           for (auto &[model, entity_collection] : model_to_entity_collection)
           {
           model->activate(); //sets up the current vertex data

           for (auto &entity : entity_collection) entity->draw();
           }
           }

        //for (transparent_materals....)*/
    }
}

