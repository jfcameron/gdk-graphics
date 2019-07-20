// © 2019 Joseph Cameron - All Rights Reserved

#include <cstdlib>

#include <gdk/camera.h>
#include <gdk/model.h>
#include <gdk/shaderprogram.h>
#include <gdk/vertexdata.h>
#include <gdk/window.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{
    SimpleGLFWWindow window("name");

    Camera camera;

    std::vector<std::shared_ptr<gdk::Model>> models;

    models.push_back(std::make_shared<Model>([&]()
    {
        auto cube = default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube));
        auto alpha = default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff));

        Model model("MySuperCoolModel",
            cube,
            alpha);

        model.setModelMatrix(Vector3<float>{0., 0., -10.}, Quaternion<float>{});

        return model;
    }()));

    //camera.setViewportSize(1.0, 1.0);

    while(true)
    {
        window.pollEvents();
        
        camera.draw(0, 0, window.GetWindowSize(), models);

        window.SwapBuffer(); 
    }

    return EXIT_SUCCESS;
}

