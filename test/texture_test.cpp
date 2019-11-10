// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/texture.h>
#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("texture constructors", "[texture]")
{
    initGL();

    auto blar = std::shared_ptr<shader_program>(shader_program::AlphaCutOff);

    SECTION("texture texture texture")
    {
        auto ptexture = static_cast<std::shared_ptr<texture>>(texture::CheckeredTextureOfDeath);

        auto b = ptexture;

        //REQUIRE(ptexture->getName() == "CheckerboardOfDeath");
    }
}

