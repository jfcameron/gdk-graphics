// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include "test_include.h"

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_texture.h>
#include <gdk/webgl1es2_shader_program.h>

using namespace gdk;

TEST_CASE("gdk::webgl1es2_material", "[gdk::webgl1es2_material]")
{
    webgl1es2_material mat(static_cast<std::shared_ptr<webgl1es2_shader_program>>(webgl1es2_shader_program::AlphaCutOff));

    SECTION("blar")
    {
        mat.setTexture("_Texture", webgl1es2_texture::GetCheckerboardOfDeath());
        
        mat.activate();
        
        REQUIRE(!jfc::glGetError());
    }
}

