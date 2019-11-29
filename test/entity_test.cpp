// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/entity.h>
#include <gdk/model.h>
#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("gdk::entity", "[gdk::entity]")
{
    initGL();

    {auto blar2 = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);}
    auto blar = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);

    SECTION("builds and draws")
    {
        entity entity(
            static_cast<std::shared_ptr<model>>(model::Cube),
            static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        entity.draw({}, {});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("move semantics")
    {
        entity a(static_cast<std::shared_ptr<model>>(model::Cube), static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        auto b = std::move(a);
    }

    SECTION("copy semantics")
    {
        const entity a(static_cast<std::shared_ptr<model>>(model::Cube), static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff));

        auto b = a;
    }
}

