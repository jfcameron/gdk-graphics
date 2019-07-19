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

//    VertexData vd("thisAtest", VertexData::Type::Static, VertexFormat::Pos3, {}, {}, VertexData::PrimitiveMode::Triangles);

    //auto p = static_cast<std::shared_ptr<VertexData>>(VertexData::Cube);

    //auto cube = default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube));


//    Model model("MySuperCoolModel",
//            cube,
//            alpha);

////////////////////////////////////////////////////////

    //std::shared_ptr<gdk::Model> pModel = std::make_shared<Model>([&]()
    //{
        //auto cube = default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube));
        //auto alpha = default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff));

        /*Model model("MySuperCoolModel",
            cube,
            alpha);

        return model;*/   

        /*Model model("MySuperCoolModel",
                default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube)),
                default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff)));

        //model.setModelMatrix((Vector3){0., 0., 0.}, (Quaternion){});

        return model;*/
    //}());


    //models.push_back(pModel);

    camera.setViewportSize(1.0, 1.0);

    camera.draw(0, 0, window.GetWindowSize(), models);

    while(true)
    {
        window.pollEvents();

        window.SwapBuffer(); 
    }

    return EXIT_SUCCESS;
}

