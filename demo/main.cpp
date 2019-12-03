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

#include <GLFW/glfw3.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{
    glfw_window window("cool demo");

    webgl1es2_camera camera;

    camera.setProjection(90, 0.01, 20, 1);

    std::vector<std::shared_ptr<gdk::webgl1es2_entity>> entities;

    auto cube = std::shared_ptr<webgl1es2_model>(webgl1es2_model::Cube);

    auto alpha = static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff);

    webgl1es2_material material(alpha);

    material.setTexture("_Texture", webgl1es2_texture::GetCheckerboardOfDeath());

    entities.push_back(std::make_shared<webgl1es2_entity>([&]()
    {
        webgl1es2_entity entity(cube, alpha);

        entity.set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());

        return entity;
    }()));

    entities.push_back(std::make_shared<webgl1es2_entity>(webgl1es2_entity(*entities.back())));

    float blar = 0;

    while(!window.shouldClose())
    {
        glfwPollEvents();
      
        auto coolEntity = entities.back();
        coolEntity->set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>{{blar,2*(blar/2),4}});
        
        camera.set_view_matrix({std::sin(blar), 0, -10}, {});

        camera.activate(window.getWindowSize());

        for(auto &current_entities : entities) 
        {
            alpha->useProgram(); //uses shader program

            cube->bind(*alpha); //binds vertex data

            current_entities->draw(camera.m_ViewMatrix, camera.m_ProjectionMatrix); //draws the data
        }

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

