// © 2019 Joseph Cameron - All Rights Reserved

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <jfc/glfw_window.h>

#include <gdk/camera.h>
#include <gdk/entity.h>
#include <gdk/shader_program.h> 
#include <gdk/model.h>

#include <GLFW/glfw3.h>

using namespace gdk;
using namespace jfc;


int main(int argc, char **argv)
{
    glfw_window window("cool demo");

    camera camera;

    camera.setProjection(90, 0.01, 20, 1);

    std::vector<std::shared_ptr<gdk::entity>> entities;

    auto cube = std::shared_ptr<model>(model::Cube);

    entities.push_back(std::make_shared<entity>([&]()
    {
        auto cube = static_cast<std::shared_ptr<model>>(model::Cube);
        auto alpha = static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff);

        //TODO: hacky. silly.
        alpha->setUniform("_Texture", *texture::GetCheckerboardOfDeath());

        entity entity(cube, alpha);

        //entity.set_texture("_Texture", texture::GetCheckerboardOfDeath());

        entity.set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());

        return entity;
    }()));

    entities.push_back(std::make_shared<entity>(entity(*entities.back())));

    float blar = 0;

    while(!window.shouldClose())
    {
        glfwPollEvents();
      
        auto mdl = entities.back();
        mdl->set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>{{blar,2*(blar/2),4}});
        
        camera.set_view_matrix({std::sin(blar), 0, -10}, {});

        camera.activate(window.getWindowSize());

        for(auto &m : entities) 
        {
            //m->activate
            m->draw(camera.m_ViewMatrix, camera.m_ProjectionMatrix);
        }

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

