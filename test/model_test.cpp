// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/webgl1es2_model.h>

using namespace gdk;
using namespace gdk::graphics;

TEST_CASE("webgl1es2_model", "[gdk::webgl1es2_model]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        /*auto pCube = model::make_cube();

        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<model>>(model::Quad);

        REQUIRE(pQuad->getHandle() >= 0);*/
    }
}

