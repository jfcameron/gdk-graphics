// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture.h>

using namespace gdk;

TEST_CASE("gdk::webgl1es2_shader_program", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    SECTION("AlphaCutOff shader initializes correctly")
    {
        auto a = static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff);

        REQUIRE(a->useProgram() != 0);
    }

    SECTION("equality semantics")
    {
        auto a = static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff);
        auto b = a;

        REQUIRE(*a == *b);
    }

    auto a = static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff);

    SECTION("can use the program in the pipeline, can assign uniform values")
    {
        a->useProgram();

        REQUIRE(!jfc::glGetError());
        
        a->setUniform("_MVP", graphics_mat4x4_type());
        a->setUniform("_DeltaTime", 1.0f);
        a->setUniform("_bCoolFeatureEnabled", true);
        
        REQUIRE(!jfc::glGetError());

        for (int i =0; i <20;++i)
        {
            a->setUniform(std::to_string(i),  *webgl1es2_texture::GetCheckerboardOfDeath());
        }
    }
}

