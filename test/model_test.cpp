// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/webgl1es2_model.h>

using namespace gdk;

TEST_CASE("webgl1es2_model", "[gdk::webgl1es2_model]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        /*auto pCube = static_cast<std::shared_ptr<model>>(model::Cube);

        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<model>>(model::Quad);

        REQUIRE(pQuad->getHandle() >= 0);*/
    }
}

