// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_shader_program.h>

using namespace gdk;

TEST_CASE("gdk::webgl1es2_entity", "[gdk::webgl1es2_entity]")
{
    initGL();

    {auto blar2 = std::shared_ptr<webgl1es2_shader_program>(webgl1es2_shader_program::AlphaCutOff);}
    auto blar = std::shared_ptr<webgl1es2_shader_program>(webgl1es2_shader_program::AlphaCutOff);

    SECTION("builds and draws")
    {
        webgl1es2_entity webgl1es2_entity(
            static_cast<std::shared_ptr<webgl1es2_model>>(webgl1es2_model::Cube),
            static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff));

        //webgl1es2_entity.draw({}, {}); //TODO: BORKED IN REFACTOR

        REQUIRE(!jfc::glGetError());
    }

    SECTION("move semantics")
    {
        webgl1es2_entity a(static_cast<std::shared_ptr<webgl1es2_model>>(webgl1es2_model::Cube), static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff));

        auto b = std::move(a);
    }

    SECTION("copy semantics")
    {
        const webgl1es2_entity a(static_cast<std::shared_ptr<webgl1es2_model>>(webgl1es2_model::Cube), static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff));

        auto b = a;
    }
}

