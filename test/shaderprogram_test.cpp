// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/shaderprogram.h>

using namespace gdk;

TEST_CASE("shaderprogram constructors", "[shaderprogram]")
{
    initGL();

    SECTION("AlphaCutOff shader initializes correctly")
    {
        auto pShader = static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff);

        //TODO: require no gl errors

        REQUIRE(pShader->getName() == "AlphaCutOff");
    }
}

