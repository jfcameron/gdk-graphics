// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/model.h>
#include <gdk/vertex_data.h>
#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("model constructors", "[model]")
{
    initGL();

    {auto blar2 = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);}
    auto blar = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);

    std::cout << "model shader: " << blar->getHandle() << std::endl;

    SECTION("blarblarblarblar")
    {
        model model("MySuperCoolmodel",
            jfc::default_ptr<vertex_data>(static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube)),
            jfc::default_ptr<shader_program>(static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff)));

        model.draw({}, {}, {}, {});

        REQUIRE(!jfc::glGetError());
    }
}

