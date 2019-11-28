// © 2019 Joseph Cameron - All Rights Reserved

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <jfc/glfw_window.h>

#include <gdk/camera.h>
#include <gdk/entity.h>
#include <gdk/shader_program.h> 
#include <gdk/vertex_data.h>

#include <GLFW/glfw3.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{
    glfw_window window("cool demo");

    camera camera;

    camera.setProjection(90, 0.01, 20, 1);

    std::vector<std::shared_ptr<gdk::entity>> models;

    auto cube = std::shared_ptr<vertex_data>(vertex_data::Cube);

    models.push_back(std::make_shared<entity>([&]()
    {
        auto cube = static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube);
        auto alpha = static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff);

        entity model(cube, alpha);

        //model.set_texture("_Texture", texture::GetCheckerboardOfDeath());

        model.set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());

        return model;
    }()));

    models.push_back(std::make_shared<entity>(entity(*models.back())));

    float blar = 0;

    while(!window.shouldClose())
    {
        glfwPollEvents();
      
        auto mdl = models.back();
        /*for (auto mdl : models)*/ mdl->set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>{{blar,2*(blar/2),4}});
        
        camera.setViewMatrix({std::sin(blar), 0, -10}, {});

        camera.draw(0, 0, window.getWindowSize(), models);

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

