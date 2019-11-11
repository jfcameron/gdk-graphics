// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertex_data.h>

using namespace gdk;

TEST_CASE("vertexdata constructors", "[vertexdata]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        /*auto pCube = static_cast<std::shared_ptr<vertex_data>>(vertex_data::Cube);

        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<vertex_data>>(vertex_data::Quad);

        REQUIRE(pQuad->getHandle() >= 0);*/
    }
}

