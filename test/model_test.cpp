// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/model.h>
#include <gdk/vertexdata.h>
#include <gdk/shaderprogram.h>

using namespace gdk;

TEST_CASE("model constructors", "[model]")
{
    initGL();

    {auto blar2 = std::shared_ptr<ShaderProgram>(ShaderProgram::AlphaCutOff);}
    auto blar = std::shared_ptr<ShaderProgram>(ShaderProgram::AlphaCutOff);

    std::cout << "model shader: " << blar->getHandle() << std::endl;

    SECTION("blarblarblarblar")
    {
        Model model("MySuperCoolModel",
            jfc::default_ptr<VertexData>(static_cast<std::shared_ptr<VertexData>>(VertexData::Cube)),
            jfc::default_ptr<ShaderProgram>(static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff)));

        model.draw({}, {}, {}, {});

        REQUIRE(!jfc::glGetError());
    }
}

