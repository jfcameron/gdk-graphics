// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("shaderprogram constructors", "[shaderprogram]")
{
    initGL();

    SECTION("AlphaCutOff shader initializes correctly")
    {
        auto pShader = static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff);

        //TODO: require no gl errors

        REQUIRE(pShader->getName() == "AlphaCutOff");
    }
}

