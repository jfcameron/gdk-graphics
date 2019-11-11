// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/model.h>
#include <gdk/vertex_data.h>
#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("gdk::model", "[gdk::model]")
{
    initGL();

    {auto blar2 = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);}
    auto blar = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);

    std::cout << "model shader: " << blar->getHandle() << std::endl;

    SECTION("builds and draws")
    {
        model model(
            static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube),
            static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        model.draw({}, {}, {}, {});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("move semantics")
    {
        model a(static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube), static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        auto b = std::move(a);
    }

    SECTION("copy semantics")
    {
        const model a(static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube), static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        auto b = a;
    }
}

