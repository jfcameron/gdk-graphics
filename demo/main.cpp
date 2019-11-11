// © 2019 Joseph Cameron - All Rights Reserved

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <jfc/window_glfw.h>

#include <gdk/camera.h>
#include <gdk/model.h>
#include <gdk/shader_program.h> 
#include <gdk/vertex_data.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{
    SimpleGLFWWindow window("cool demo");

    camera camera;

    camera.setProjection(90, 0.01, 20, 1);

    std::vector<std::shared_ptr<gdk::model>> models;

    auto cube = std::shared_ptr<vertex_data>(vertex_data::Cube);

    models.push_back(std::make_shared<model>([&]()
    {
        auto cube = default_ptr<vertex_data>(static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube));
        auto alpha = default_ptr<shader_program>(static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        model model("MySuperCoolModel",
            cube,
            alpha);

        model.set_texture("_Texture", texture::GetCheckerboardOfDeath());

        model.set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>());

        return model;
    }()));

    float blar = 0;

    while(!window.shouldClose())
    {
        window.pollEvents();
       
        for (auto mdl : models) mdl->set_model_matrix(Vector3<float>{0., 0., -11.}, Quaternion<float>{{blar,2*(blar/2),4}});
        
        camera.setViewMatrix({std::sin(blar), 0, -10}, {});

        camera.draw(0, 0, window.getWindowSize(), models);

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

