// © 2019 Joseph Cameron - All Rights Reserved

#include <cmath>
#include <cstdlib>

#include <jfc/window.h>

#include <gdk/perspective_camera.h>
#include <gdk/model.h>
#include <gdk/shaderprogram.h>
#include <gdk/vertexdata.h>

using namespace gdk;
using namespace jfc;

int main(int argc, char **argv)
{
    SimpleGLFWWindow window("name");

    perspective_camera camera;

    std::vector<std::shared_ptr<gdk::Model>> models;

    models.push_back(std::make_shared<Model>([&]()
    {
        auto cube = default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube));
        auto alpha = default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff));

        Model model("MySuperCoolModel",
            cube,
            alpha);

        model.setTexture("_Texture", default_ptr<Texture>(static_cast<std::shared_ptr<Texture>>(Texture::CheckeredTextureOfDeath)));

        model.setModelMatrix(Vector3<float>{0., 0., -10.}, Quaternion<float>{});

        return model;
    }()));

    float blar = 0;

    while(!window.shouldClose())
    {
        window.pollEvents();
        
        camera.setViewMatrix({std::sin(blar), 0, -10}, {});

        camera.draw(0, 0, window.getWindowSize(), models);

        window.swapBuffer(); 

        blar += 0.001;
    }

    return EXIT_SUCCESS;
}

