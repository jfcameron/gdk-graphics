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

    std::shared_ptr<gdk::Model> pModel = std::make_shared<Model>([&]()
    {
        Model model("MySuperCoolModel",
                default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube)),
                default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff)));

        //model.setModelMatrix((Vector3){0., 0., 0.}, (Quaternion){});

        return model;
    }());

    std::vector<std::shared_ptr<gdk::Model>> models;

    models.push_back(pModel);

    camera.setViewportSize(1.0, 1.0);

    camera.draw(0, 0, window.GetWindowSize(), models);

    //TODO: integrate findings into learnings about window stuff. NO_POLL_UPDATE? hmmm. maybe.

    //glfwPollEvents(); //has to be called at least once interestingly...

    while(true)
    {
        window.pollEvents();

        window.SwapBuffer(); 
    }

    return EXIT_SUCCESS;
}

