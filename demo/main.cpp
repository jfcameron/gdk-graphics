// © 2019 Joseph Cameron - All Rights Reserved

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <jfc/glfw_window.h>

#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_shader_program.h> 
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_material.h>

//abstraction work...
#include <gdk/graphics_context.h>

#include <GLFW/glfw3.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{   
    // Separate lib, used to init GL and get a window ready for rendering
    glfw_window window("cool demo");

    // specifying the library's implementation to be used
    //TODO: continue expanding context methods. eventually remove static_casts, virtualize minimum methods required
    auto pContext = graphics::context::make(graphics::context::implementation::opengl_webgl1_gles2);

    auto pCamera = std::dynamic_pointer_cast<webgl1es2_camera>(std::shared_ptr<gdk::camera>(std::move(pContext->make_camera())));
    pCamera->setProjection(90, 0.01, 20, 1);
    
    auto pAlpha = std::static_pointer_cast<webgl1es2_shader_program>(pContext->get_alpha_cutoff_shader());

    auto pCube = std::static_pointer_cast<webgl1es2_model>(pContext->get_cube_model());

    auto pMaterial2 = std::make_shared<webgl1es2_material>(pAlpha);
    pMaterial2->setTexture("_Texture", webgl1es2_texture::GetCheckerboardOfDeath());

    auto pMaterial = std::make_shared<webgl1es2_material>(pAlpha);
    pMaterial->setTexture("_Texture", webgl1es2_texture::GetTestTexture());

    std::vector<std::shared_ptr<gdk::webgl1es2_entity>> entities;

    entities.push_back(std::make_shared<webgl1es2_entity>([&]()
    {
        webgl1es2_entity entity(pCube, pMaterial);

        entity.set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());

        return entity;
    }()));

    entities.push_back(std::make_shared<webgl1es2_entity>(webgl1es2_entity(*entities.back())));

    float blar = 0;

    while(!window.shouldClose())
    {
        glfwPollEvents();
      
        auto coolEntity = entities.back();
        coolEntity->set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>{{blar, 2 * (blar / 2), 4}});
        
        pCamera->set_view_matrix({std::sin(blar), 0, -10}, {});

        pCamera->activate(window.getWindowSize());

        for(auto &current_entities : entities) 
        {
            static bool yepp = false;

            if (yepp)
            {
                pMaterial2->activate();

                yepp = false;
            }
            else
            {
                pMaterial->activate();

                yepp = true;
            }

            pCube->bind(*pAlpha); //binds vertex data

            current_entities->draw(pCamera->m_ViewMatrix, pCamera->m_ProjectionMatrix); //draws the data
        }

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

